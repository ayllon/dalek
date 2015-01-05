/*
 * screen.h
 */
#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <types.h>

#define T_VIDEO   0xB8000
#define T_COLUMNS 80
#define T_ROWS    25

#define TAB_WIDTH 4

/* Character attributes */
#define BLACK          0x0
#define BLUE           0x1
#define GREEN          0x2
#define CYAN           0x3
#define RED            0x4
#define MAGENTA        0x5
#define YELLOW         0x6
#define LIGHT_GREY     0x7
#define DARK_GREY      0x8
#define LIGHT_BLUE     0x9
#define LIGHT_GREEN    0xA
#define LIGHT_CYAN     0xB
#define LIGHT_RED      0xC
#define LIGHT_MANGENTA 0xD
#define LIGHT_YELLOW   0xE
#define WHITE          0xF

/* Default attributes */
#define T_ATTR    (WHITE | (BLUE << 4))

/* Terminal primitives */
void screen_clear();
void screen_scroll(uint8_t n);
void screen_updatecursor();
char screen_putc(char c);

#endif // __SCREEN_H__
