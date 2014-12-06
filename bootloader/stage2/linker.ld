ENTRY(start)
SECTIONS
{
  . = 0x0500;
  .text : { */stage2.S.o(.text) *(.text) }
  .data : { *(.data) }
  end = .; _end = .; __end = .;
}
