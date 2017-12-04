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

	/* Go for long mode */
	call    setup_page_tables
	call    enable_paging

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

/* Setup page tables */
setup_page_tables:
    // Map first P4 entry to P3 table
    mov     $p3_table, %eax
    // present + writable
    or      $0b11, %eax
    mov     %eax, p4_table

    // map first P3 entry to P2 table
    mov     $p2_table, %eax
    // present + writable
    or      $0b11, %eax
    mov     %eax, p3_table

    // Map each P2 entry to a huge 2MiB page
    mov     $0x0, %ecx

.map_p2_table:
    // map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
    mov     $0x200000, %eax
    mul     %ecx
    // present + writable + huge
    or      $0b10000011, %eax
    mov     %eax, p2_table(,%ecx, 8)

    inc     %ecx
    cmp     $512, %ecx
    jne     .map_p2_table
    ret

/* Enable pages */
enable_paging:
    mov     $p4_table, %eax
    mov     %eax, %cr3

    // Enable PAE
    mov     %cr4, %eax
    or      $(1 << 5), %eax
    mov     %eax, %cr4

    // Set long mode bit
    mov     $0xC0000080, %ecx
    rdmsr
    or      $(1 << 8), %eax
    wrmsr

    // Enable paging
    mov     %cr0, %eax
    or      $(1 << 31), %eax
    mov     %eax, %cr0

    ret

.bss
.align 4096
p4_table:
    .skip 4096
p3_table:
    .skip 4096
p2_table:
    .skip 4096
stack_bottom:
    .skip 64
