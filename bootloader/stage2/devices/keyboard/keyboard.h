/*
 * keyboard.h
 *
 * Keyboard handling
 *
 */
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include <types.h>

#define KEY_ESC      0x0100
#define KEY_LCONTROL 0x0200
#define KEY_LSHIFT   0x0300
#define KEY_RSHIFT   0x0400
#define KEY_CAPS     0x0500
#define KEY_F1       0x0600
#define KEY_F2       0x0700
#define KEY_F3       0x0800
#define KEY_F4       0x0900
#define KEY_F5       0x0A00
#define KEY_F6       0x0B00
#define KEY_F7       0x0C00
#define KEY_F8       0x0D00
#define KEY_F9       0x0E00
#define KEY_F10      0x0F00
#define KEY_NUMLOCK  0x1000
#define KEY_SCROLL   0x1100
#define KEY_F11      0x1200
#define KEY_F12      0x1300
#define KEY_LALT     0x1400
#define KEY_RCONTROL 0x1500
#define KEY_PRTSCN   0x1600
#define KEY_RALT     0x1700
#define KEY_BREAK    0x1800
#define KEY_HOME     0x1900
#define KEY_UP       0x2000
#define KEY_PGUP     0x2100
#define KEY_LEFT     0x2200
#define KEY_RIGHT    0x2300
#define KEY_END      0x2400
#define KEY_DOWN     0x2500
#define KEY_PGDOWN   0x2600
#define KEY_INSERT   0x2700
#define KEY_DELETE   0x2800
#define KEY_PRNTSCN  0x2900

#define KEY_ENTER    '\n'

#define KB_BUFFER_LEN 32

#define KB_IRQ 1

/** METHODS **/
void kb_init(void);
uint16_t kb_getc();

#endif
