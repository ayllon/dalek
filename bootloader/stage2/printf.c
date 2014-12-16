/*
 * printf.c
 *
 * Functions to print strings
 */
#include <printf.h>
#include <stdarg.h>
#include <strings.h>
#include <memory.h>
#include <ports.h>

static struct {
    uint8_t *video;
    uint8_t attr, oldattr;
    uint16_t posx, posy;
} screen = { (uint8_t*) T_VIDEO, T_ATTR, 0, 0 };

/**
 * Clear the terminal
 */
void cls()
{
    uint16_t i = 0;

    for (i = 0; i < T_COLUMNS * T_ROWS; i++) {
        *(screen.video + i * 2) = 0x00;
        *(screen.video + i * 2 + 1) = screen.attr;
    }

    screen.posx = 0;
    screen.posy = 0;
    updatecursor();
}

/**
 * Scrolls the terminal
 */
void scroll(uint8_t n)
{
    uint16_t i;

    if (n > T_ROWS)
        n = T_ROWS;
    if (n == 0)
        return;

    // 2 bytes per character (color and character)
    memcpy(screen.video, screen.video + T_COLUMNS * n * 2,
            (T_ROWS - n) * T_COLUMNS * 2);

    // Clear last line
    i = (T_ROWS - n) * T_COLUMNS;
    while (i < (T_ROWS * T_COLUMNS)) {
        *(screen.video + i * 2) = 0x00;
        *(screen.video + i * 2 + 1) = screen.attr;
        i++;
    }

    screen.posy = T_ROWS - n;
    screen.posx = 0;
    updatecursor();
}

/**
 * Changes the current output color
 */
void setcolor(uint8_t forecolor, uint8_t backcolor)
{
    screen.oldattr = screen.attr;
    screen.attr = forecolor | (backcolor << 4);
}

/**
 * Sets the front color
 */
void setforecolor(uint8_t forecolor)
{
    screen.oldattr = screen.attr;
    screen.attr &= 0xF0;
    screen.attr |= forecolor;
}

/**
 * Puts the color information into the variables
 */
void getcolor(uint8_t *forecolor, uint8_t *backcolor)
{
    *forecolor = screen.attr & 0x0F;
    *backcolor = (screen.attr & 0xF0) >> 4;
}

/**
 * Restore the previous color
 */
void restorecolor()
{
    screen.attr = screen.oldattr;
}

/**
 * Update the cursor position
 */
void updatecursor()
{
    static uint16_t position;
    static uint8_t val;
    position = screen.posx + screen.posy * T_COLUMNS;

    // Set low byte
    val = position & 0x00FF;
    outportb(0x03D4, 0x0F); // Register index
    outportb(0x03D5, val);
    // Set high byte
    val = (position & 0xFF00) >> 8;
    outportb(0x03D4, 0x0E); // Register index
    outportb(0x03D5, val);
}

/**
 * Puts a character into the console
 * c   The character
 */
char putc(char c)
{
    // New line
    switch (c) {
    case '\n':
        screen.posx = 0;
        if (++(screen.posy) >= T_ROWS)
            scroll(1);
        break;
    case '\r':
        screen.posx = 0;
        break;
    case '\b':
        if (screen.posx > 0) {
            screen.posx--;
            putc('\0');
            screen.posx--;
        }
        break;
    case '\t':
        // Next multiple of TAB_WIDTH
        screen.posx = ((screen.posx / TAB_WIDTH) + 1) * TAB_WIDTH;
        if (screen.posx > T_COLUMNS) {
            screen.posx = 0;
            screen.posy++;
            if (screen.posy >= T_ROWS)
                scroll(1);
        }
        break;
    default:
        // Put char
        *(screen.video + (screen.posx + screen.posy * T_COLUMNS) * 2) = c
                & 0xFF;
        *(screen.video + (screen.posx + screen.posy * T_COLUMNS) * 2 + 1) =
                screen.attr;

        if (++(screen.posx) > T_COLUMNS) {
            screen.posx = 0;
            screen.posy++;
            if (screen.posy >= T_ROWS)
                scroll(1);
        }
    }
    // Update cursor
    updatecursor();
    // Return
    return c;
}

/**
 * Just a wrapper for vprintf
 */
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
    unsigned int i;
    static char buffer[64], *p;

    for (i = 0; *s; s++) {
        // Field
        if (*s != '%') {
            putc(*s);
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
                putc((char) (va_arg(args, int)));
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
                    putc(*p++);
                break;
            default:
                putc(*s);
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
    int i = 0;
    switch (level) {
        case LOG_WARN:
            setforecolor(LIGHT_BROWN);
            break;
        default:
            setforecolor(LIGHT_GREY);
    }

    i += printf("[%s] ", func);
    va_list args;
    va_start(args, msg);
    i += vprintf(msg, args);
    va_end(args);
    printf("\n");
    restorecolor();
    return i;
}
