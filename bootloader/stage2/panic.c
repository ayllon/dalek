/*
 * panic.c
 *
 */
#include <panic.h>
#include <printf.h>
#include <stdarg.h>

void panic(const char *s, ...)
{
  int a;
  va_list args;

  setcolor(LIGHT_RED, BLACK);
  printf("PANIC!! ");
  va_start(args, s);
  vprintf(s, args);
  va_end(args);

  printf("\nSystem freeze...\n");

  asm("cli");
  while(1);
  asm("hlt");
}
