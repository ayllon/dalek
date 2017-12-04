.text
.code64

.global long_mode_start
.extern rust_main

long_mode_start:
    // Reload data segments
    mov     $0x0, %ax
    mov     %ax, %ss
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs

    // Call rust
    call    rust_main

    // We are 64 bits!
    hlt
