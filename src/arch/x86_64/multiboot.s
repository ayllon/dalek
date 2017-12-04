// Multiboot header
.section .multiboot
multiboot_header:
    // Magic number
    .long   0xe85250d6
    // Architecture (0 = i386)
    .long   0x0
    // Header length
    .long   multiboot_header_end - multiboot_header
    // Checksum
    .long   0x100000000 - (0xe85250d6 + 0 + (multiboot_header_end - multiboot_header))

    // Tags

    // End tag
    // Type
    .word  0
    // Flags
    .word  0
    // Size
    .long  8
multiboot_header_end:
