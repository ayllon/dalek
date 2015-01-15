/*
 * screen.c
 */
#include <io.h>
#include <memory.h>
#include <modules.h>
#include <ports.h>
#include <stdio.h>
#include <strings.h>
#include "screen.h"


static volatile struct {
    uint8_t *video;
    uint8_t attr, oldattr;
    uint16_t posx, posy;
} screen_info = { (uint8_t*) T_VIDEO, T_ATTR, T_ATTR, 0 };


/**
 * Clear the terminal
 */
void screen_clear()
{
    uint16_t i = 0;

    for (i = 0; i < T_COLUMNS * T_ROWS; i++) {
        *(screen_info.video + i * 2) = 0x00;
        *(screen_info.video + i * 2 + 1) = screen_info.attr;
    }

    screen_info.posx = 0;
    screen_info.posy = 0;
    screen_updatecursor();
}


/**
 * Scrolls the terminal
 */
void screen_scroll(uint8_t n)
{
    uint16_t i;

    if (n > T_ROWS)
        n = T_ROWS;
    if (n == 0)
        return;

    // 2 bytes per character (color and character)
    memcpy(screen_info.video, screen_info.video + T_COLUMNS * n * 2,
            (T_ROWS - n) * T_COLUMNS * 2);

    // Clear last line
    i = (T_ROWS - n) * T_COLUMNS;
    while (i < (T_ROWS * T_COLUMNS)) {
        *(screen_info.video + i * 2) = 0x00;
        *(screen_info.video + i * 2 + 1) = screen_info.attr;
        i++;
    }

    screen_info.posy = T_ROWS - n;
    screen_info.posx = 0;
    screen_updatecursor();
}


/**
 * Update the cursor position
 */
void screen_updatecursor()
{
    uint16_t position;
    uint8_t val;
    position = screen_info.posx + screen_info.posy * T_COLUMNS;

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
char screen_putc(char c)
{
    // New line
    switch (c) {
    case '\n':
        screen_info.posx = 0;
        if (++(screen_info.posy) >= T_ROWS)
            screen_scroll(1);
        break;
    case '\r':
        screen_info.posx = 0;
        break;
    case '\b':
        if (screen_info.posx > 0) {
            screen_info.posx--;
            screen_putc('\0');
            screen_info.posx--;
        }
        break;
    case '\t':
        // Next multiple of TAB_WIDTH
        screen_info.posx = ((screen_info.posx / TAB_WIDTH) + 1) * TAB_WIDTH;
        if (screen_info.posx > T_COLUMNS) {
            screen_info.posx = 0;
            screen_info.posy++;
            if (screen_info.posy >= T_ROWS)
                screen_scroll(1);
        }
        break;
    default:
        // Put char
        *(screen_info.video + (screen_info.posx + screen_info.posy * T_COLUMNS) * 2) = c
                & 0xFF;
        *(screen_info.video + (screen_info.posx + screen_info.posy * T_COLUMNS) * 2 + 1) =
                screen_info.attr;

        if (++(screen_info.posx) > T_COLUMNS) {
            screen_info.posx = 0;
            screen_info.posy++;
            if (screen_info.posy >= T_ROWS)
                screen_scroll(1);
        }
    }
    // Update cursor
    screen_updatecursor();
    // Return
    return c;
}

/**
 * Handle ANSI set attributes
 */
static void screen_ansi_attr(const char* params, size_t len)
{
    static const uint8_t color_table[] = {
        BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, LIGHT_GREY
    };

    long attr = 0;
    if (len > 0) {
        attr = atoi(params);
    }

    uint8_t oldattr = screen_info.attr;
    char* p = (char*)params;

    do {
        attr = strtol(p, &p, 10);

        // Reset
        if (attr == 0) {
            screen_info.attr = screen_info.oldattr;
        }
        // Bold
        else if (attr == 1) {
            screen_info.attr |= 0x08;
        }
        // Foreground
        else if (attr >= 30 && attr <= 37) {
            screen_info.attr &= 0xF0;
            screen_info.attr |= (color_table[attr - 30]);
        }
        // Background
        else if (attr >= 40 && attr <= 47) {
            screen_info.attr &= 0x0F;
            screen_info.attr |= ((color_table[attr - 40]) << 4);
        }

        if (p)
            ++p;
    } while (p && (p - params) < len);

    screen_info.oldattr = oldattr;
}

/**
 * ANSI codes (partial) support
 */
static const char* screen_handle_ansi(const char* p, const char* end)
{
    // Only support sequences starting with [
    ++p;
    if (p > end || *p != '[')
        return p;

    // Get command
    const char* params = p + 1;
    do {
        ++p;
    } while (p < end && *p && !(*p > '@' && *p < '~'));

    // End of string?
    if (p > end || *p == '\0')
        return p;

    switch (*p) {
        // Clear screen (only full screen supported now)
        case 'J':
            screen_clear();
            break;
        // Attributes
        case 'm':
            screen_ansi_attr(params, p - params);
            break;
    }

    return p + 1;
}

/**
 * Writes to the screen
 */
static ssize_t screen_write(IODevice* self, const void* buffer, size_t nbytes)
{
    const char* p = (const char*)buffer;
    const char* end = p + nbytes;

    while (p != end) {
        // ANSI codes
        if (*p == 0x1B) {
            p = screen_handle_ansi(p, end);
        }
        // Just write the character
        else {
            screen_putc(*p);
            ++p;
        }
    }
    return nbytes;
}

/**
 * Initialize the screen
 */
int screen_init(void)
{
    IODevice *screen = io_register_device("screen", "Screen", NULL);
    screen->write = screen_write;
    screen_clear();

    // Attach itself to stdout
    stdout = screen;
    return 0;
}

MODULE_INIT_LVL(screen_init, 0);
