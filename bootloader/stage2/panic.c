/*
 * panic.c
 *
 */
#include <panic.h>
#include <printf.h>
#include <stdarg.h>

void panic(const char* f, const char *s, ...)
{
    va_list args;

    setcolor(LIGHT_RED, BLACK);
    printf("PANIC!! ");
    printf("[%s] ", f);

    va_start(args, s);
    vprintf(s, args);
    va_end(args);

    printf("\nHalting...\n");

    asm("cli");
    while (1)
        asm("hlt");
}
