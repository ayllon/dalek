OUTPUT_FORMAT("binary")
ENTRY(start)
SECTIONS
{
  . = 0x0500;
  .text : { *(.text) }
  .data : { *(.data) }
  /*.bss  : { *(.bss)  }*/
  end = .; _end = .; __end = .;
}