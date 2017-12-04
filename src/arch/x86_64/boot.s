.text
.code32

.global start

/* Entry point */
start:
    /* Set stack
	 * There should be 30KiB available here
	 * http://wiki.osdev.org/Memory_Map_%28x86%29
	 */
	mov		$0x00007BFF, %esp
	mov		%esp, %ebp

	/* Do some cross checks */
	call    check_multiboot
	call    check_cpuid
	call    check_longmode

    movl    $0x2f4b2f4f, 0xb8000
    hlt

/* Verify that EAX contains the magic value that a multiboot
   loader should have left */
check_multiboot:
    cmp     $0x36d76289, %eax
    jne     .no_multiboot
    ret
.no_multiboot:
    mov     $'M', %al
    jmp     error

/* Verify the CPU implements CPUID by flipping the bit 21 in the flags
 * From http://wiki.osdev.org/Setting_Up_Long_Mode#Detection_of_CPUID
 * Via https://os.phil-opp.com/entering-longmode/
 */
check_cpuid:
    // Get flags into EAX via stack
    pushfl
    pop %eax

    // Copy to ECX as well for comparing later on
    mov %eax, %ecx

    // Flip the ID bit
    xor $(1 << 21), %eax

    // Copy EAX to FLAGS via the stack
    push %eax
    popfl

    // Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
    pushfl
    pop %eax

    // Restore FLAGS from the old version stored in ECX (i.e. flipping the
    // ID bit back if it was ever flipped).
    push %ecx
    popfl

    // Compare EAX and ECX. If they are equal then that means the bit
    // wasn't flipped, and CPUID isn't supported.
    cmp %eax, %ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov $'C', %al
    jmp error

/* Verify long mode (64 bits) is supported
 * From http://wiki.osdev.org/Setting_Up_Long_Mode#x86_or_x86-64
 * Via https://os.phil-opp.com/entering-longmode/
 */
check_longmode:
    // test if extended processor info in available
    mov $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jb .no_long_mode

    // use extended info to test if long mode is available
    mov $0x80000001, %eax
    cpuid
    test $(1 << 29), %edx
    jz .no_long_mode
    ret
.no_long_mode:
    mov $'L', %al
    jmp error

/* Print an error code (in AL) and halts */
error:
    movl    $0x4f524f45, 0xb8000
    movl    $0x4f3a4f52, 0xb8004
    movl    $0x4f204f20, 0xb8008
    movw    $0x4f00, 0xb800a
    movb    %al, 0xb800a
    hlt
