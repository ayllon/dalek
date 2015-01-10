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
bpbOEM:					.byte	'D','A','l','e','K',' ',' ',' '
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


/* Continue */
loader:
	/* Reset segment registers */
	xor	%ax, %ax
	mov	%ax, %ds
	mov	%ax, %es

	/* Store boot drive */
	mov %dl, bootDrive

	/* Just say we are here :-) */
	mov		$strLoading, %si
	call	print

	/* Reset floppy */
	call	floppyReset

	/* Read FAT Table */
	mov		$_end, %bx
	mov		%bx, fatPointer
	call	readFAT

	/* Read root directory */
	add		fatPointer, %ax
	mov		%ax, rootPointer
	mov		rootPointer, %bx
	call	readRoot

	/* Search for the file */
	call	searchFile
	or		%ax, %ax
	jz		_notFound

	/* Load file to memory */
	/* Where */
	push	%ax
	mov		$0x8000, %bx

	mov		bpbBytesPerSector, %ax
	mulw	bpbSectorsPerCluster
	mov		%ax, bytesPerCluster

	pop		%ax

_stage2_read:
	/* Read */
	push	%ax

	call	clusterLBA

	mov		bpbSectorsPerCluster, %cl
	xor		%dx, %dx
	call	readLBA

	pop		%ax
	mov		$strDot, %si
	call	print
	/* Get next cluster from FAT*/
	call	nextCluster
	add		bytesPerCluster, %bx
	cmp		$0x0FF0, %ax
	jb		_stage2_read

	/* Jump there */
	xor		%bx, %bx
	mov		bootDrive, %bl	/* Boot drive */
	ljmp	$0x0000,$0x8000

	/* Halt */
_notFound:
	mov		$strNotFound, %si
	call	print
	cli
	hlt

/* print function
 * Register SI: Pointer to the string
 */
print:
	pusha
	xor		%bh, %bh
	mov		$0x0e, %ah
_print_loop:
	lodsb
	or		%al, %al
	jz		_print_done
	int		$0x10
	jmp		_print_loop
_print_done:
	popa
	ret


/* Reset floppy
 */
floppyReset:
	pusha
_flReset:
	xor		%ah, %ah
	mov		bootDrive, %dl
	int		$0x13
	jc		_flReset /* Until we get it */
	popa
	ret

/* Read a logical sector from the disk using BIOS interrupts
 * (Convert secuential cluster to Head, Cylinder, Sector and read)
 * AX = Logical sector
 * CL = Number of logical sectors
 * ES:BX = Where to put the data
 */
readLBA:
	mov	%cl, _nsect
	pusha

	/* Logical sector / Sectors per track */
	divw	bpbSectorsPerTrack
	mov		%dl, _sector
	incb	_sector		/* Now we have the sector */

	/* (LS / SPT) mod heads */
	xor		%dx, %dx
	divw	bpbHeadsPerCylinder
	mov		%dl, _head	/* Here we have the head */
	/* LS / (SPT * NH) */
	mov		%al, _cyl	/* And the cylinder */

_readLBA_loop:
	mov		bootDrive, %dl
	mov		_nsect,	 %al
	mov		_cyl,    %ch
	mov		_sector, %cl
	mov		_head,   %dh

	mov		$0x02, %ah
	int		$0x13
	jc		_readLBA_loop

	popa
	call	floppyReset
	ret

_sector:
	.byte	0x00
_head:
	.byte	0x00
_cyl:
	.byte	0x00
_nsect:
	.byte	0x00

/* Read FAT Table
 * Puts FAT table in ES:BX, returns FAT size (in bytes) in DX.AX
 */
readFAT:
	pusha
	/* Where is the 1st FAT table? */
	mov		bpbReservedSectors, %ax	/* Sector number        */
	mov		$0x00, %dx		/* Sector number (high) */

	/* Read FAT Table */
	mov		bpbSectorsPerFAT, %cx	/* Size */
	call	readLBA

	popa

	/* FAT Size */
	mov		bpbBytesPerSector, %ax
	mulw	bpbSectorsPerFAT

	ret

/* Read root directory
 * Read root directory from the disk into
 * the memory area pointed by ES:BX
 */
readRoot:
	pusha
	/* Size of root (in sectors)*/
	mov		$32, %ax		/* 32 bytes per entry */
	mulw	bpbRootEntries
	divw	bpbBytesPerSector
	xor		%cx, %cx
	mov		%al, %cl		/* In CL we have now nº of sectors */

	/* Where is Root? */
	mov		bpbNumberOfFATs, %al
	xor		%ah, %ah
	mulw	bpbSectorsPerFAT
	add		bpbReservedSectors, %ax	/* Sector number        */
	adc		$0x00, %dx		/* Sector number (high) */

	/* Calculate the begin of the data */
	mov		%ax, dataSector
	add		%cx, dataSector

	/* Read */
	call	readLBA

	popa
	ret

/* Search for file
 * Returns the first cluster (0 if not found or empty) in AX
 */
searchFile:
	mov		rootPointer, %di
	mov		bpbRootEntries, %cx
_searchLoop:
	push	%cx
	mov		$11, %cx
	mov		$stage2File, %si
	push	%di
    rep 	cmpsb
	pop		%di
	pop		%cx
	je		_searchMatch
	add		$32, %di		/* Next entry */
	loop	_searchLoop
	/* Not found */
	xor		%ax, %ax
	ret
	/* Found :-D */
_searchMatch:
	mov		0x1a(%di), %ax
	ret

/* Get next cluster
 * Process FAT table looking for the next cluster
 * AX = Current cluster
 * Returns the next cluster in AX, 0x0FF0/0x0FFF if none
 */
nextCluster:
	push	%bx
	push	%cx
	push	%dx

	/* Get the offset in the table*/
	mov		%ax, %cx
	mov		%ax, %dx
	shr		$0x01, %dx
	add		%dx, %cx
	/* Read word */
	mov		fatPointer, %bx
	add		%cx, %bx
	mov		(%bx), %dx
	/* Get only the bits we need */
	test	$0x0001, %ax
	jnz		_next_ODD
	/* Even cluster */
_next_EVEN:
	and		$0b0000111111111111, %dx
	jmp		_next_Done
	/* Odd cluster */
_next_ODD:
	shr		$0x04, %dx
	/* End */
_next_Done:
	mov		%dx, %ax

	pop		%dx
	pop		%cx
	pop		%bx
	ret

/* Cluster to LBA
 * AX = Cluster
 * Leave in AX the logical sector
 */
clusterLBA:
	push	%cx

	sub		$0x02, %ax	/* Cluster 0x02 means 0 :-P */
	xor		%cx, %cx
	mov		bpbSectorsPerCluster, %cl
	mul		%cx
	add		dataSector, %ax	/* Cluster 0 is next to root directory */

	pop		%cx
	ret


/* Variables */
fatPointer:
	.word	0x00
rootPointer:
	.word	0x00
bootDrive:
	.byte	0x00
dataSector:
	.word	0x00
bytesPerCluster:
	.word	0x00

/* Strings */
strLoading:
	.string "Loading Stage 2\n\r"
strNotFound:
	.string "Stage 2 file not found\n\r"
strDot:
	.string "."
stage2File:
	.byte 'S','T','A','G','E','2',' ',' ','B','I','N'

/* Magic */
	.org 0x1FE
	.word 0xAA55

_end:
