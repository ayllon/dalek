/*
 * screen.h
 */
#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <types.h>

#define T_VIDEO   0xB8000
#define T_COLUMNS 80
#define T_ROWS    25
#define T_ATTR    0x07

#define TAB_WIDTH 4

/* Character attributes */
#define BLACK          0x0
#define BLUE           0x1
#define GREEN          0x2
#define CYAN           0x3
#define RED            0x4
#define MAGENTA        0x5
#define BROWN          0x6
#define LIGHT_GREY     0x7
#define DARK_GREY      0x8
#define LIGHT_BLUE     0x9
#define LIGHT_GREEN    0xA
#define LIGHT_CYAN     0xB
#define LIGHT_RED      0xC
#define LIGHT_MANGENTA 0xD
#define LIGHT_BROWN    0xE
#define WHITE          0xF

/* Terminal primitives */
void screen_clear();
void screen_scroll(uint8_t n);
void screen_setcolor(uint8_t forecolor, uint8_t backcolor);
void screen_getcolor(uint8_t *forecolor, uint8_t *backcolor);
void screen_setforecolor(uint8_t forecolor);
void screen_restorecolor();
void screen_updatecursor();
char screen_putc(char c);

#endif // __SCREEN_H__
