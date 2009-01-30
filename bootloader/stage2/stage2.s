/*
 * stage2.s
 *
 * Second stage boot loader
 *
 */

.code16
.text
.global start

/* Start */
start:
	cli
	/* Ensure segment register are OK */
	push	%cs
	pop	%ds
	
	/* Just say hello :-) */
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
	
	xor	%bh, %bh
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
	