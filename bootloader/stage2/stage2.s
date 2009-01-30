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
	/* Reset segment registers */
	xor	%ax, %ax
	mov	%ax, %ds
	mov	%ax, %es

	/* Just say we are here :-) */
	mov	$strHello, %si
	call	print

	/* Halt */
	cli
	hlt

/* print function
 * Register SI: Pointer to the string
 */
print:
	push	%ax
	push	%bx
	
	xor	%bx, %bx
	mov	$0x0e, %ah
_print_loop:	
	lodsb
	or	%al, %al
	jz	_print_done
	int	$0x10
	jmp	_print_loop
_print_done:
	
	pop	%bx
	pop	%ax
	ret

/* Strings */
strHello:
	.string "DAleK Loader [Stage 2]\n\rDoing nothing.\n\r"
	