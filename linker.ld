ENTRY(start)

SECTIONS {
    . = 1M;

    .text :
    {
        /* Multiboot header at the beginning */
        KEEP(*(.multiboot))
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data.rel.ro : {
        *(.data.rel.ro.local*) *(.data.rel.ro .data.rel.ro.*)
    }

    .data : {
        *(.data.* .data)
    }
}
