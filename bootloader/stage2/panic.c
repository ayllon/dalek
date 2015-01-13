/*
 * panic.c
 *
 */
#include <panic.h>
#include <stdarg.h>
#include <stdio.h>

void* __stack_chk_guard = (void*)0x11caec4;


void panic(const char* f, const char *s, ...)
{
    va_list args;

    //setcolor(LIGHT_RED, BLACK);
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

// Called when a stack smash is detected
void __stack_chk_fail(void)
{
    panic(__func__, "Stack smash detected!");
}
