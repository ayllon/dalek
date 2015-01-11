.global chainload_asm

/* Abandon protected mode and jump to 0x7c00 */
.code32
chainload_asm:
	/* Recover drive number */
	mov		4(%esp), %eax
	mov		%al, drive_number
	/* No interrupts now! */
	cli
	/* New stack */
	mov		$0x6F2C, %esp

	/* Set real mode segments */
	mov		$0x20, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		%ax, %fs
	mov		%ax, %gs
	mov		%ax, %ss

	/* Jump to 16 bits code */
	ljmp	$0x18,$chainload_asm_16

.code16
chainload_asm_16:
	/* Clear protected mode and paginations bits */
	mov		%cr0, %eax
	and		$0x7FFFFFE, %eax
	mov		%eax, %cr0

	/* Trigger a reload of CS */
	ljmp	$0x00,$chainload_asm_16_real

chainload_asm_16_real:
	/* Real mode! Reset segments */
	xor		%ax, %ax
	mov		%ax, %ds
	mov		%ax, %es
	mov		%ax, %fs
	mov		%ax, %gs
	mov		%ax, %ss

	/* Setup idt */
	lidt	real_idt

	/* Farewell */
	mov		drive_number, %dl
	sti
	ljmp	$0x00,$0x7C00

drive_number:
.byte	0x00
real_idt:
.word	0x03FF // Limit
.word	0x0000 // Base high
.word	0x0000 // Base low
