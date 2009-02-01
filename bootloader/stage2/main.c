/* Main file */
#include <types.h>
#include <printf.h>

void main(uint16 drive)
{
  cls();
  setcolor(RED, BLACK);
  printf("Stage 2 loaded from drive %i\n", drive);
  asm("hlt");
}
