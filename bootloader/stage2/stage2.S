/*
 * stage2.S
 *
 * Second stage boot loader
 * Set everything ready and jump to C code
 * In BX we have the boot drive
 */

.code16
.text
.global start
.global original_processor_flags

/* Start */
start:
	cli
	movb	%dl, boot_drive

	/* Save flags */
	pushf
	popw	original_processor_flags

	/* Set stack
	 * There should be 30KiB available here
	 * http://wiki.osdev.org/Memory_Map_%28x86%29
	 */
	mov		$0x00007BFF, %esp
	mov		%esp, %ebp

	/* Setup segments */
	xorw	%ax, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	sti

	/* Set the GDT */
	lgdt	gdt_toc

	/* Get the memory map */
	call 	load_memory_map

	/* Switch to protected mode (32 bits) */
	cli
	mov		%cr0, %eax
	or		$0x01, %eax
	mov		%eax, %cr0
	ljmp	$0x08,$stage_32

/* Ask BIOS for the memory layout and stores it into
 * boot_information
  */
load_memory_map:
	mov		$str_query_memory_map, %si
	call	print16

	// Reset ebx and bp
	xor		%ebx, %ebx
	push	%bp
	xor		%bp, %bp
	// Where to write
	mov		$memory_map_entries, %di
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
	jc		_load_memory_map_fail
	// EAX must be SMAP, or it failed
	cmp		%edx, %eax
	jne		_load_memory_map_fail
	// If EBX is 0, the table is not good
	test	%ebx, %ebx
	je		_load_memory_map_fail
	// Just to generate feedback
	mov		$str_memory_map_progress, %si
load_memory_mapLoop:
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
	je		load_memory_mapDone
	jnc		load_memory_mapLoop

load_memory_mapDone:
	mov		%bp, memory_memory_map_n_entries_info
	pop		%bp
	mov		$str_memory_map_done, %si
	call	print16
	ret

_load_memory_map_fail:
	mov		$str_memory_map_failed, %si
	call	print16
_memory_map_failed_loop:
	hlt
	jmp		_memory_map_failed_loop

str_query_memory_map:
	.string "\n\rQuerying memory map\n\r"
str_memory_map_done:
	.string "Done\n\r"
str_memory_map_failed:
	.string "Failed to get the memory map!\n\r"
str_memory_map_progress:
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
stage_32:
	/* Set segments */
	mov		$0x10, %ax
	mov		%ax, %ds
	mov		%ax, %ss
	mov		%ax, %es

	/* Enable A20 */
	mov		$0xdd, %al
	out		%al, $0x64

	/* Jump */
	push	$boot_information
	call	main

/* GDT */
gdt_table:
	#include "gdt.S"

gdt_toc:
	.word	gdt_toc - gdt_table - 1	/* Size of GDT */
	.int	gdt_table		/* Base of GDT */

/* Struct passed to main */
.code32
original_processor_flags:
	.word	0
boot_information:
boot_drive:
	.byte	0
memory_map_info:
memory_memory_map_n_entries_info:
	.word	0
memory_map_entries:
	.zero 768
