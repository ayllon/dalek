/*
 * stage2.S
 *
 * Second stage boot loader
 * Set everything ready and jump to C code
 * In BX we have the boot drive
 */

#define ASM 1
#include <memory.h>

.code16
.text
.global start

/* Start */
start:
	cli
	mov		%bx, bootDrive
	/* Setup segments and stack */
	xor		%ax, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		$0x9000, %ax
	mov		%ax, %ss
	mov		$0xFFFF, %sp
	sti

	/* Set the GDT */
	lgdt	gdtToc

	/* Get the memory map */
	call 	loadMemoryMap

	/* Switch to protected mode (32 bits) */
	cli
	mov		%cr0, %eax
	or		$0x01, %eax
	mov		%eax, %cr0
	ljmp	$0x08,$stage3

/* Ask BIOS for the memory layout and stores it into
 * bootInformation
  */
loadMemoryMap:
	mov		$memoryMapMsg, %si
	call	print16

	// Reset ebx and bp
	xor		%ebx, %ebx
	push	%bp
	xor		%bp, %bp
	// Where to write
	mov		$memoryMapEntries, %di
	// Force ACPI 3.0 use bit to 1
	movl	$1, 20(%di)
	// BIOS function
	mov		$0xE820, %eax
	// "SMAP" into edx
	mov		$0x534D4150, %edx
	// 24 bytes each entry
	mov		$24, %ecx
	// Call BIOS
	int		$0x15
	// If carry bit is set, the  operation failed
	jc		loadMemoryMapFail
	// EAX must be SMAP, or it failed
	cmp		%edx, %eax
	jne		loadMemoryMapFail
	// If EBX is 0, the table is not good
	test	%ebx, %ebx
	je		loadMemoryMapFail
	// Just to generate feedback
	mov		$memoryMapFeedback, %si
loadMemoryMapLoop:
	inc		%bp
	call 	print16
	add 	$24, %di
	mov 	$0xE820, %eax
	mov 	$24, %ecx
	mov 	$0x534D4150, %edx
	// Force ACPI 3.0 use bit to 1
	movl	$1, 20(%di)
	int		$0x15
	test	%ebx, %ebx
	je		loadMemoryMapDone
	jnc		loadMemoryMapLoop

loadMemoryMapDone:
	mov		%bp, memoryMapNbEntries
	pop		%bp
	mov		$memoryMapEndMsg, %si
	call	print16
	ret

loadMemoryMapFail:
	mov		$memoryMapFailMsg, %si
	call	print16
memoryMapFailedLoop:
	hlt
	jmp		memoryMapFailedLoop

memoryMapMsg:
	.string "Querying memory map\r\n"
memoryMapEntryMsg:
	.string "Got a segment\r\n"
memoryMapEndMsg:
	.string "Done\r\n"
memoryMapFailMsg:
	.string "Failed to get the memory map!\r\n"
memoryMapFeedback:
	.string "."

/* print function, handy for seeing what's going on
 * Register SI: Pointer to the string
 */
print16:
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

/* Stage 3 point. This is in 32 bits! */
.code32
stage3:
	/* Set segments */
	mov		$0x10, %ax
	mov		%ax, %ds
	mov		%ax, %ss
	mov		%ax, %es

	/* Set stack */
	mov		$(stack + STACK_SIZE), %esp
	mov		%esp, %ebp

	/* Enable A20 */
	mov		$0xDD, %al
	out		%al, $0x64

	/* Jump */
	push	$bootInformation
	call	main

/* GDT */
gdtTable:
	#include "gdt.S"

gdtToc:
	.word	gdtToc - gdtTable -1	/* Size of GDT */
	.int	gdtTable		/* Base of GDT */

/* Struct passed to main */
.code16
bootInformation:
bootDrive:
	.word	0
memoryMapInfo:
memoryMapNbEntries:
	.word	0
memoryMapEntries:
	.zero 768

/* Stack */
.comm	stack, STACK_SIZE
