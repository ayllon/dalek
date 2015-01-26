OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(start)
SECTIONS
{
  . = 0x7e00;
  .text : { */stage2.S.o(.text) *(.text) }
  .data : { *(.data) }
  .rodata : { *(.rodata) }
  
  __k_mod_init : {
    __start___k_mod_init = .;
    *(SORT(__k_mod_init_*))
    __stop___k_mod_init = .;
  }
  __k_mod_deinit : {
  	__start___k_mod_destroy = .;
  	*(SORT(__k_mod_deinit_*))
  	__stop___k_mod_destroy = .;
  }
  
  end = .; _end = .; __end = .;
}
