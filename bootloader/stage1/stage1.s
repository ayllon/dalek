/*
 * stage1.s
 *
 * First stage boot loader
 * This file looks for and loads the second stage boot loader
 * It depends on the file system (this one is for FAT12/16),
 * in the future I plan to write for others.
 */

.code16
.text
.global start



/* Start */
start:
	jmp loader
	nop
	
/* FAT Header */
bpbOEM:			.byte	'D','A','l','e','K',' ',' ',' '
bpbBytesPerSector:	.word	512
bpbSectorsPerCluster:	.byte	1
bpbReservedSectors:	.word	1
bpbNumberOfFATs:	.byte	2
bpbRootEntries:		.word	224
bpbTotalSectors: 	.word	2880
bpbMedia:		.byte	0xF0
bpbSectorsPerFAT:	.word	9
bpbSectorsPerTrack:	.word	18
bpbHeadsPerCylinder:	.word	2
bpbHiddenSectors:	.word	0,0
bpbTotalSectorsBig:	.word	0,0
bsDriveNumber:		.byte	0
bsUnused:		.byte	0
bsExtBootSignature:	.byte	0x29
bsSerialNumber:		.word	0xa0a1, 0xa2a3
bsVolumeLabel:		.byte	'B','O','O','T',' ','F','L','O','P','P','Y'
bsFileSystem:		.byte	'F','A','T','1','2',' ',' ',' '


/* Continue */
loader:	
	/* Reset segment registers */
	xor	%ax, %ax
	mov	%ax, %ds
	mov	%ax, %es

	/* Just say we are here :-) */
	mov	$strLoading, %si
	call	print

	/* Reset floppy */
	mov	$0, %dl
	call	floppyReset

	/* Read FAT Table */
	mov	fatPointer, %bx
	call	readFAT

	/* Read root directory */
	add	fatPointer, %ax
	mov	%ax, rootPointer
	mov	rootPointer, %bx
	mov	$0, %dl
	call	readRoot
	
	/* Search for the file */
	mov	rootPointer, %si
	call	print
	
	#mov	$stage2File, %si
	#call	searchFile
	
	/* Load to memory */
	#call	readLBA
	
	/* Jump there */

	/* Halt */
	cli
	hlt

/* print function
 * Register SI: Pointer to the string
 */
print:
	pusha
	
	xor	%bx, %bx
	mov	$0x0e, %ah
_print_loop:	
	lodsb
	or	%al, %al
	jz	_print_done
	int	$0x10
	jmp	_print_loop
_print_done:
	
	popa
	ret


/* Reset floppy
 * In DL must be the floppy unit (usually, 0)
 */
floppyReset:
	push	%ax
_flReset:	
	xor	%ah, %ah
	int	$0x13
	jc	_flReset /* Until we get it */
	pop	%ax
	ret

/* Read a logical sector CX from the disk CH using BIOS interrupts
 * (Convert secuential cluster to Head, Cylinder, Sector and read)
 * DX.AX = Logical sector
 * CL = Number of logical sectors
 * CH = Drive number
 * ES:BX = Where to put the data
 */
readLBA:
	pusha
	
	/* Logical sector / Sectors per track */
	divw	bpbSectorsPerTrack
	mov	%ax, _sector
	incw	_sector		/* Now we have the sector */
	
	/* (LS / SPT) mod heads */
	divw	bpbHeadsPerCylinder
	mov	%dx, _head	/* Here we have the head */

	/* LS / (SPT * NH) */
	push	%ax
	push	%dx
	mov	bpbSectorsPerTrack, %ax
	mulb	bpbHeadsPerCylinder
	mov	%ax, _cyl
	pop	%dx
	pop	%ax
	divw	_cyl
	mov	%ax, _cyl	/* And the cylinder */

	popa
	
	/* Read */
	pusha
_readLBA_loop:
	mov	%ch, %dl /* Drive */
	mov	%cl, %al /* Size */
	
	mov	_cyl,    %ch
	mov	_sector, %cl
	mov	_head,   %dh
	
	
	mov	$0x02, %ah
	int	$0x13
	jc	_readLBA_loop
	
	popa
	ret
_sector:
	.byte	0x00
_head:
	.byte	0x00
_cyl:
	.byte	0x00

/* Read FAT Table
 * In DL is the drive number
 * Puts FAT table in ES:BX, returns FAT size (in bytes) in DX.AX
 */
readFAT:
	pusha
	mov	%dl, %ch
	/* Where is the 1st FAT table? */
	mov	bpbReservedSectors, %ax	/* Cluster number        */
	mov	$0x00, %dx		/* Cluster number (high) */
	
	/* Read FAT Table */
	mov	bpbSectorsPerFAT, %cl	/* Size */
	call	readLBA

	popa
	
	/* FAT Size */
	mov	bpbBytesPerSector, %ax
	mulw	bpbSectorsPerFAT

	ret

/* Read root directory
 * Read root directory from the disk DL into
 * the memory area pointed by ES:BX
 */
readRoot:
	mov	%dl, _drive
	pusha
	/* Where is Root? */
	mov	bpbReservedSectors, %cx
	mov	bpbNumberOfFATs, %ax
	mulw	bpbSectorsPerFAT
	add	%ax, %cx		/* Cluster number        */
	adc	$0x00,%dx		/* Cluster number (high) */

	/* Size of root (in sectors)*/
	push	%dx
	mov	bpbRootEntries, %ax
	mov	$32, %dl		/* 32 bytes per entry */
	mul	%dl
	divw	bpbBytesPerSector	/* In AL we have now nº of sectors */
	pop	%dx

	/* Read */
	mov	_drive, %dl
	call	readLBA
	
	popa
	ret
_drive:
	.byte	0x00

/* Search for file
 * Search for the file pointed by BX in the root directory pointed by SI
 * Returns the first cluster (0 if not founded) in AX
 */
searchFile:
	ret

/* Gets next cluster
 * Process FAT table looking for the next cluster
 * AX = Current cluster
 * SI = FAT table in memory
 * Returns the next cluster in AX
 */
nextCluster:
	ret

	
/* Variables */
fatPointer:
	.word _end
rootPointer:
	.word 0x00
	
/* Strings */
strLoading:
	.string "DAleK Loader [Stage 1.1]\n\r"
strNoFile:
	.string "Stage 2 file not found\n\r"
stage2File:
	.byte 'S','T','A','G','E','2',' ',' ','B','I','N'
	
/* Magic */
	#.org 0x1FE
	.word 0xAA55

_end:
	