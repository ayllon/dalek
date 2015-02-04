/*
 * stage1.s
 *
 * First stage boot loader
 * This file looks for and loads the second stage boot loader
 * It depends on the file system (this one is for FAT12/16).
 */

.code16
.text
.global start


/* Start */
start:
	jmp loader
	nop

/* FAT Header */
bpbOEM:					.byte	'D','A','L','E','K',' ',' ',' '
bpbBytesPerSector:		.word	512
bpbSectorsPerCluster:	.byte	1
bpbReservedSectors:		.word	1
bpbNumberOfFATs:		.byte	2
bpbRootEntries:			.word	224
bpbTotalSectors:		.word	2880
bpbMedia:				.byte	0xF0
bpbSectorsPerFAT:		.word	9
bpbSectorsPerTrack:		.word	18
bpbHeadsPerCylinder:	.word	2
bpbHiddenSectors:		.word	0,0
bpbTotalSectorsBig:		.word	0,0
bsDriveNumber:			.byte	0
bsUnused:				.byte	0
bsExtBootSignature:		.byte	0x29
bsSerialNumber:			.word	0xa0a1, 0xa2a3
bsVolumeLabel:			.byte	'B','O','O','T',' ','F','L','O','P','P','Y'
bsFileSystem:			.byte	'F','A','T','1','2',' ',' ',' '

/* Reset segment registers and stack */
loader:
	xorw	%ax, %ax
	movw	%ax, %ss
	movw	%ax, %ds
	movw	%ax, %es
	movw	$0x7bff, %sp
	ljmp	$0x00,$loader2

/* print function
 * %si Pointer to the string
 */
print:
	pusha
	xorb	%bh, %bh
	movb	$0x0e, %ah
_print_loop:
	lodsb
	orb		%al, %al
	jz		_print_done
	int		$0x10
	jmp		_print_loop
_print_done:
	popa
	ret

/* Panic */
panic:
	call	print
	cli
_panic_loop:
	hlt
	jmp		_panic_loop

/* Reset disk
 */
disk_reset:
	pusha
_disk_reset_retry:
	xorb	%ah, %ah
	movb	bsDriveNumber, %dl
	int		$0x13
	jc		_disk_reset_retry
	popa
	ret

/* Read a logical sector from the disk using BIOS interrupts
 * %ax 		Logical sector
 * %cl		Number of sectors
 * %es:%bx	Where to put the data
 */
read_sectors:
	pusha

_read_sectors_loop:
	movw	%ax, _logical_sector
	movb	%cl, _nsect

	// Logical sector / Sectors per track
	xorw	%dx, %dx
	divw	bpbSectorsPerTrack
	movb	%dl, _sector
	incb	_sector

	// (LS / SPT) mod heads
	xorw	%dx, %dx
	divw	bpbHeadsPerCylinder
	movb	%dl, _head
	// LS / (SPT * NH)
	movb	%al, _cyl

	// %ah contains the high 2 bits of cyl
	shlb	$6, %ah
	orb		%ah, _sector

	movb	$3, _retries
_read_sectors_retry:
	movb	_retries, %al
	orb 	%al, %al
	jz		_read_sectors_panic
	decb	_retries

	mov		bsDriveNumber, %dl
	mov		$1,	 %al
	mov		_cyl, %ch
	mov		_sector, %cl
	mov		_head, %dh

	mov		$0x02, %ah
	int		$0x13
	jc		_read_sectors_retry

	addw	bpbBytesPerSector, %bx

	movw	_logical_sector, %ax
	incw	%ax
	xorw	%cx, %cx
	movb	_nsect, %cl
	loop	_read_sectors_loop

	popa
	ret

_read_sectors_panic:
	movw	$str_read_panic, %si
	jmp		panic

_sector:
	.byte	0x00
_head:
	.byte	0x00
_cyl:
	.byte	0x00
_nsect:
	.byte	0x00
_retries:
	.byte	0x00
_logical_sector:
	.word	0x00

/* Ask the BIOS for the disk parameters
 * http://stanislavs.org/helppc/int_13-8.html
 */
disk_get_params:
	pusha

	movb	bsDriveNumber, %dl
	movb	$0x08, %ah
	int		$0x13

	jc		_disk_params_panic

	// Low 6 bits in %cl is sectors per track
	andb	$0b00111111, %cl
	movb	%cl, bpbSectorsPerTrack

	xorw	%ax, %ax
	movb	%dh, %al
	incw	%ax
	movw	%ax, bpbHeadsPerCylinder

	popa
	ret

_disk_params_panic:
	movw	$str_disk_params_panic, %si
	jmp		panic

/* Loader */
loader2:
	movb	%dl, bsDriveNumber
	movw	$str_welcome, %si
	call	print

	// Reset disk
	call	disk_reset

	// Do not trust the information in the FAT header
	call	disk_get_params

	// Determine where does the list start, iterate sector_range_count times
	movw	sector_range_count, %cx
	orw		%cx, %cx
	jz		_self_check_panic

	movw	$4, %ax
	mulw	%cx
	movw	$sector_range_count, %si
	subw	%ax, %si

	movw	$0x07e0, %ax
	movw	%ax, %es
	xorw	%bx, %bx

_load_loop:
	// Print a dot to give feedback
	push	%si
	movw	$str_progress, %si
	call	print
	popw	%si

	// Save %cx
	pushw	%cx

	// Read the sector (%ax) and the number of sectors (%cx)
	movw	(%si), %ax
	movw	2(%si), %cx
	call	read_sectors

	// Next memory area
	movw	bpbBytesPerSector, %ax
	mulw	%cx
	addw	%ax, %bx

	// Restore %cx
	popw	%cx

	// Next sector chain
	addw	$4, %si

	// Repeat
	loop	_load_loop

	// %si must be pointing to sector_range_count
	movw	$sector_range_count, %ax
	cmpw	%ax, %si
	jne		_self_check_panic

	// Jump to stage2
	ljmp	$0x07e0,$0x0000

_self_check_panic:
	movw	$str_self_check_panic, %si
	jmp		panic

/* Variables and strings */
str_welcome:
	.string	"Loading stage 2\n\r"
str_progress:
	.string	"."
str_read_panic:
	.string	"\n\rFailed to read from disk"
str_self_check_panic:
	.string	"\n\rSelf-check failed"
str_disk_params_panic:
	.string "\r\nFailed to get the disk parameters"

/* Sector list goes here */

/* Here the installer puts the number of sectors */
.org 0x1FC
sector_range_count:
	.word	0x00

/* Magic */
.org 0x1FE
.word 0xAA55

