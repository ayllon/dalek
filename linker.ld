ENTRY(start)

SECTIONS {
    . = 1M;

    .boot BLOCK(4K) : ALIGN(4K)
    {
        /* Multiboot header at the beginning */
        KEEP(*(.multiboot))
    }

    .text :
    {
        *(.text)
    }
}
