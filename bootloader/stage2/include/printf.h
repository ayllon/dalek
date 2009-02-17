/*
 * printf.h
 *
 * Functions to print string to the screen
 */
#ifndef __PRINTF_H__
#define __PRINTF_H__

#include <stdarg.h>
#include <types.h>

#define T_VIDEO   0xB8000
#define T_COLUMNS 80
#define T_ROWS    25
#define T_ATTR	  0x07

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

void cls();
void scroll(uint8 n);
void setcolor(uint8 forecolor, uint8 backcolor);
void getcolor(uint8 *forecolor, uint8 *backcolor);
void restorecolor();
void updatecursor();
char putc(char c);
int  vprintf(const char *s, va_list args);
int  printf(const char *s, ...);

#endif 
