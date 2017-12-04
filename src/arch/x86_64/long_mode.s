.text
.code64

.global long_mode_start

long_mode_start:
    // Reload data segments
    mov     $0x0, %ax
    mov     %ax, %ss
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs

    // We are 64 bits!
    mov $0x2f592f412f4b2f4f, %rax
    mov %rax, 0xb8000
    hlt
