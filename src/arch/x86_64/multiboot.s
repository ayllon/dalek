// Multiboot header
.section .multiboot
.align 8
.code32
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
framebuffer_tag:
    // Type
    .short 0
    // Flags
    .short 0
    // Size
    .long  20
    // Width
    .long  80
    // Height
    .long  25
    // Depth
    .long  2
// Align
    .long  0

end_tag:
    // Type
    .short 0
    // Flags
    .short 0
    // Size
    .long  8
multiboot_header_end:
