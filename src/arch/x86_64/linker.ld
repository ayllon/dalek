ENTRY(start)

SECTIONS {
    . = 1M;

    .boot :
    {
        /* Multiboot header at the beginning */
        *(.multiboot)
    }

    .text :
    {
        *(.text)
    }
}
