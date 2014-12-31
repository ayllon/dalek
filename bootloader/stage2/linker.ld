OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(start)
SECTIONS
{
  . = 0x0500;
  .text : { */stage2.S.o(.text) *(.text) }
  .data : { *(.data) }
  .rodata : { *(.rodata) }
  
  __k_cli_cmd : {
    __start___k_cli_cmd = .;
    *(SORT(__k_cli_cmd_*))
    __stop___k_cli_cmd = .;
  }
  
  end = .; _end = .; __end = .;
}
