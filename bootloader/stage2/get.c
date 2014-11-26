/*
 * get.c
 *
 */
#include <get.h>
#include <keyboard.h>
#include <printf.h>

/**
 * Read a character from the keyboard
 */
char getc()
{
    static uint16_t c = 0;
    // Only ASCII
    while ((c = kb_getc()) > 0xFF);

    return c & 0xFF;
}

/**
 * Read a string from the keyboard
 */
char *gets(char *dest)
{
    static uint16_t i;
    static char c;

    i = 0;

    // Get character
    do {
        c = getc();

        // Actions
        switch (c) {
        case '\b': // Delete
            if (i > 0) {
                dest[--i] = '\0';
                putc(c); // Echo
            }
            break;
        case '\n': // EOL
            dest[i] = '\0';
            putc(c);
            break;
        default: // Rest
            putc(dest[i++] = c);
        }
    } while (c != '\n'); // Until EOL

    // Return
    return dest;
}
