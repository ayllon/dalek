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
    NULL,    // 0x00
    KEY_ESC, //0x01
    '1' , '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', 0xAD, '\b', // 0x0E
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '`', '+',    // 0x1B
    KEY_ENTER,    // 0x1C
    KEY_LCONTROL, // 0x1D
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0xA4, '\'', 0xA7, // 0x29
    KEY_LSHIFT, // 0x2A
    0x87,       // 0x2B
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-', // 0x35
    KEY_RSHIFT, // 0x36
    NULL,       // 0x37
    KEY_LALT,   // 0x38
    ' ',        // 0x39
    KEY_CAPS,   // 0x3a
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, // 0x44
    KEY_NUMLOCK, KEY_SCROLL, // 0x46
    '7', '8', '9',    // 0x49
    '-',              // 0x4a
    '4', '5', '6',    // 0x4d
    '+',              // 0x4e
    '1', '2', '3',    // 0x51
    '0', '.',         // 0x53
    NULL, NULL, '<',  // 0x56
    KEY_F11, KEY_F12, // 0x58
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x5F
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x67
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x6F
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x77
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL  // 0x7F
    };

uint16 kb_shift_keymap[] = 
  {
    NULL,
    KEY_ESC,
    '!' , '"', 0xFA, '$', '%', '&', '/', '(', ')', '=', '?', 0xA8, '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '^', '*',
    KEY_ENTER,
    KEY_LCONTROL,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 0xA5, ' ', 0xA6,
    KEY_LSHIFT,
    0x80,
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
    NULL, NULL, '>',  // 0x56
    KEY_F11, KEY_F12, // 0x58
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x5F
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x67
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x6F
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x77
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL  // 0x7F
    };

uint16 kb_altgr_keymap[] = 
  {
    NULL,
    KEY_ESC,
    '|' , '@', '#', '~', 0xAB, 0xAA, '{', '[', ']', '}', '\'', '\\', '\b',
    '\t', '@', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', '[', ']',
    KEY_ENTER,
    KEY_LCONTROL,
    0x91, 0xE1, 'd', 'f', 'g', 'h', 'j', 'k', 'l', '~', '{', '\\',
    KEY_LSHIFT,
    '}',
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
    NULL, NULL, '<',  // 0x56
    KEY_F11, KEY_F12, // 0x58
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x5F
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x67
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x6F
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x77
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL  // 0x7F
    };

uint16 kb_escaped_keymap[] = 
  {
    NULL,    // 0x00
    NULL, //0x01
    NULL , NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x0E
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '`', '+',    // 0x1B
    KEY_ENTER,    // 0x1C
    KEY_RCONTROL, // 0x1D
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x29
    KEY_LSHIFT, // 0x2A
    NULL,       // 0x2B
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '/', // 0x35
    KEY_RSHIFT, // 0x36
    KEY_PRNTSCN,// 0x37
    KEY_RALT,   // 0x38
    NULL,       // 0x39
    NULL,       // 0x3a
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x44
    NULL, KEY_BREAK,  // 0x46
    KEY_HOME, KEY_UP, KEY_PGUP,    // 0x49
    NULL,             // 0x4a
    KEY_LEFT, NULL, KEY_RIGHT,     // 0x4d
    NULL,             // 0x4e
    KEY_END, KEY_DOWN, KEY_PGDOWN, // 0x51
    KEY_INSERT, KEY_DELETE,        // 0x53
    NULL, NULL, NULL,  // 0x56
    NULL, NULL, // 0x58
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x5F
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x67
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x6F
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x77
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL  // 0x7F
    };

