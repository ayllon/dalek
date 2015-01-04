/*
 * printf.c
 *
 * Functions to print strings
 */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <strings.h>
#include <memory.h>
#include <ports.h>

IODevice* stdin = NULL;
IODevice* stdout = NULL;


char getc()
{
    if (!stdin) {
        errno = EBADF;
        return -1;
    }
    char c;
    stdin->read(stdin, &c, 1);
    return c & 0xFF;
}


char *gets(char *dest)
{
    uint16_t i;
    char c;

    i = 0;

    // Get character
    do {
        c = getc();
        if (c < 0)
            return NULL;

        // Actions
        switch (c) {
        case '\b': // Delete
            if (i > 0) {
                dest[--i] = '\0';
                stdout->write(stdout, &c, 1); // Echo
            }
            break;
        case '\n': // EOL
            dest[i] = '\0';
            stdout->write(stdout, &c, 1);
            break;
        default: // Rest
            dest[i++] = c;
            stdout->write(stdout, &c, 1);
        }
    } while (c != '\n'); // Until EOL

    // Return
    return dest;
}


int printf(const char *s, ...)
{
    int a;
    va_list args;
    va_start(args, s);
    a = vprintf(s, args);
    va_end(args);
    return a;
}

/**
 * Prints a formated string in the console
 * s    The formated string
 * args The arguments
 */
int vprintf(const char *s, va_list args)
{
    if (!stdout)
        return 0;
    unsigned int i;
    char buffer[64], *p;

    for (i = 0; *s; s++) {
        // Field
        if (*s != '%') {
            buffer[0] = *s;
            stdout->write(stdout, buffer, 1);
        }
        else {
            unsigned lcount = 0;
            char pad = 0;
            int w = 0;
format:
            switch (*(++s)) {
            case '0':
                if (!pad) {
                    pad = '0';
                    goto format;
                }
                /* no break */
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                w = w * 10 + (*s - '0');
                goto format;
            case 'l':
                ++lcount;
                goto format;
            case 'c':
                buffer[0] = va_arg(args, int);
                stdout->write(stdout, buffer, 1);
                break;
            case 'd':
            case 'i':
                if (lcount < 2)
                    itoa_s(va_arg(args, int), 10, pad, w, buffer, sizeof(buffer));
                else
                    itoa_s(va_arg(args, long long int), 10, pad, w, buffer, sizeof(buffer));
                p = buffer;
                goto string;
            case 'x':
            case 'X':
                if (lcount < 2)
                    itoa_s(va_arg(args, int), 16, pad, w, buffer, sizeof(buffer));
                else
                    itoa_s(va_arg(args, long long int), 16, pad, w, buffer, sizeof(buffer));
                p = buffer;
                goto string;
            case 'z':
                itoa_s(va_arg(args, size_t), 10, pad, w, buffer, sizeof(buffer));
                p = buffer;
                goto string;
            case 'p':
                itoa_s((long) va_arg(args, void*), 16, '0',
                        sizeof(void*) * 2, buffer, sizeof(buffer));
                p = buffer;
                goto string;
            case 'f':
                ftoa_s(va_arg(args, double), 2, buffer, sizeof(buffer));
                p = buffer;
                goto string;
            case 's':
                p = va_arg(args, char*);
                string: while (*p)
                    stdout->write(stdout, p++, 1);
                break;
            default:
                stdout->write(stdout, p, 1);
            }
        }
    }

    return i;
}

/**
 * Prints a log entry
 */
int log(int level, const char* func, const char* msg, ...)
{
    int i = 0;/*
    switch (level) {
        case LOG_WARN:
            setforecolor(LIGHT_BROWN);
            break;
        default:
            setforecolor(LIGHT_GREY);
    }
*/
    i += printf("[%s] ", func);
    va_list args;
    va_start(args, msg);
    i += vprintf(msg, args);
    va_end(args);
    printf("\n");
    //restorecolor();
    return i;
}
