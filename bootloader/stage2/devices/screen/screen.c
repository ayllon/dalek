/*
 * screen.c
 */
#include <io.h>
#include <ports.h>
#include <memory.h>
#include "screen.h"


static volatile struct {
    uint8_t *video;
    uint8_t attr, oldattr;
    uint16_t posx, posy;
} screen = { (uint8_t*) T_VIDEO, T_ATTR, 0, 0 };


/**
 * Clear the terminal
 */
void screen_clear()
{
    uint16_t i = 0;

    for (i = 0; i < T_COLUMNS * T_ROWS; i++) {
        *(screen.video + i * 2) = 0x00;
        *(screen.video + i * 2 + 1) = screen.attr;
    }

    screen.posx = 0;
    screen.posy = 0;
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
    screen_updatecursor();
}


/**
 * Changes the current output color
 */
void screen_setcolor(uint8_t forecolor, uint8_t backcolor)
{
    screen.oldattr = screen.attr;
    screen.attr = forecolor | (backcolor << 4);
}


/**
 * Sets the front color
 */
void screen_setforecolor(uint8_t forecolor)
{
    screen.oldattr = screen.attr;
    screen.attr &= 0xF0;
    screen.attr |= forecolor;
}


/**
 * Puts the color information into the variables
 */
void screen_getcolor(uint8_t *forecolor, uint8_t *backcolor)
{
    *forecolor = screen.attr & 0x0F;
    *backcolor = (screen.attr & 0xF0) >> 4;
}


/**
 * Restore the previous color
 */
void screen_restorecolor()
{
    screen.attr = screen.oldattr;
}


/**
 * Update the cursor position
 */
void screen_updatecursor()
{
    uint16_t position;
    uint8_t val;
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
char screen_putc(char c)
{
    // New line
    switch (c) {
    case '\n':
        screen.posx = 0;
        if (++(screen.posy) >= T_ROWS)
            screen_scroll(1);
        break;
    case '\r':
        screen.posx = 0;
        break;
    case '\b':
        if (screen.posx > 0) {
            screen.posx--;
            screen_putc('\0');
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
                screen_scroll(1);
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
                screen_scroll(1);
        }
    }
    // Update cursor
    screen_updatecursor();
    // Return
    return c;
}

/**
 * Writes to the screen
 */
ssize_t screen_write(IODevice* self, void* buffer, size_t nbytes)
{
    size_t i;
    char* cbuffer = (char*)buffer;
    for (i = 0; i < nbytes; ++i) {
        screen_putc(cbuffer[i]);
    }
    return i;
}

/**
 * Initialize the screen
 */
void screen_init(void)
{
    IODevice *screen = io_register_device("screen", "Screen", NULL);
    screen->write = screen_write;
    screen_setcolor(WHITE, BLUE);
    screen_clear();
}

REGISTER_IO_EARLY(screen_init);
