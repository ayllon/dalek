/*
 * es_es.c
 *
 * Keymap for Spanish (spain)
 *
 * This file must be compilated independently (as pure binary!)
 * or included as builtin keymap
 */
#include <types.h>
#include <keyboard.h>

uint16 kb_keymap[] = 
  {
    NULL,
    KEY_ESC,
    '1' , '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', 0xAD, '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '`', '+',
    KEY_ENTER,
    KEY_LCONTROL,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0xA4, '\'', 0x87,
    KEY_LSHIFT,
    '<',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-',
    KEY_RSHIFT,
    NULL,
    KEY_LALT,
    ' ',
    KEY_CAPS,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
    KEY_NUMLOCK, KEY_SCROLL,
    '7', '8', '9',
    '-',
    '4', '5', '6',
    '+',
    '1', '2', '3',
    '0', '.',
    NULL, NULL, NULL,
    KEY_F11, KEY_F12
    // There are more
    };

uint16 kb_shift_keymap[] = 
  {
    NULL,
    KEY_ESC,
    '!' , '"', 0xFA, '$', '%', '&', '/', '(', ')', '=', '?', 0xA8, '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '^', '*',
    KEY_ENTER,
    KEY_LCONTROL,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 0xA5, ' ', 0x80,
    KEY_LSHIFT,
    '>',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_',
    KEY_RSHIFT,
    NULL,
    KEY_LALT,
    ' ',
    KEY_CAPS,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
    KEY_NUMLOCK, KEY_SCROLL,
    '7', '8', '9',
    '-',
    '4', '5', '6',
    '+',
    '1', '2', '3',
    '0', '.',
    NULL, NULL, NULL,
    KEY_F11, KEY_F12
    // There are more
    };

uint16 kb_altgr_map[] = 
  {
    NULL,
    KEY_ESC,
    '|' , '@', '#', '~', 0xAB, 0xAA, '{', '[', ']', '}', '\'', '\\', '\b',
    '\t', '@', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', '[', ']',
    KEY_ENTER,
    KEY_LCONTROL,
    0x91, 0xE1, 'd', 'f', 'g', 'h', 'j', 'k', 'l', '~', '{', '}',
    KEY_LSHIFT,
    '|',
    'z', 'x', 'c', '"', '"', 'n', 'm', ';', 0xAB, '-',
    KEY_RSHIFT,
    NULL,
    KEY_LALT,
    ' ',
    KEY_CAPS,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
    KEY_NUMLOCK, KEY_SCROLL,
    '7', '8', '9',
    '-',
    '4', '5', '6',
    '+',
    '1', '2', '3',
    '0', '.',
    NULL, NULL, NULL,
    KEY_F11, KEY_F12
    // There are more
    };

