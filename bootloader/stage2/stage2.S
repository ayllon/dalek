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

	/* Set the GDT */
	lgdt	gdtToc

	/* Switch to protected mode (32 bits) */
	cli
	mov	%cr0, %eax
	or	$0x01, %eax
	mov	%eax, %cr0
	ljmp	$0x08,$stage3

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

	/* Jump */
	push	bootDrive
	call	main

/* GDT */
gdtTable:
	.include "gdt.S"

gdtToc:
	.word	gdtToc - gdtTable -1	/* Size of GDT */
	.int	gdtTable		/* Base of GDT */

/* Variables */
bootDrive:
	.word	0x0000
/* Stack */
	.comm	stack, STACK_SIZE
