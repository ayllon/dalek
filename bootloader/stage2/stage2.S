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
	mov	%bx, bootDrive
	/* Setup segments and stack */
	xor	%ax, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	$0x9000, %ax
	mov	%ax, %ss
	mov	$0xFFFF, %sp
	sti

	/* Get the memory map */
	call loadMemoryMap

	/* Set the GDT */
	lgdt	gdtToc

	/* Switch to protected mode (32 bits) */
	cli
	mov	%cr0, %eax
	or	$0x01, %eax
	mov	%eax, %cr0
	ljmp	$0x08,$stage3

/* Ask BIOS for the memory layout and stores it into
 * bootInformation
  */
loadMemoryMap:
	mov $memoryMapMsg, %si
	call print16
	mov $memoryMapEndMsg, %si
	call print16
	ret

memoryMapMsg:
	.string "Querying memory map\r\n"
memoryMapEntryMsg:
	.string "Got a segment\r\n"
memoryMapEndMsg:
	.string "Done\r\n"

/* print function, handy for seeing what's going on
 * Register SI: Pointer to the string
 */
print16:
	pusha
	xor	%bh, %bh
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

/* Stage 3 point. This is in 32 bits! */
.code32
stage3:
	/* Set segments */
	mov	$0x10, %ax
	mov	%ax, %ds
	mov	%ax, %ss
	mov	%ax, %es

	/* Set stack */
	mov	$(stack + STACK_SIZE), %esp
	mov	%esp, %ebp

	/* Enable A20 */
	mov	$0xDD, %al
	out	%al, $0x64

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
	.word 0
.lcomm	memoryMapEntries, 8191

/* Stack */
	.comm	stack, STACK_SIZE
