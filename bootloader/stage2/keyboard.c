/*
 * keyboard.h
 *
 * Keyboard handling
 *
 */
#include <keyboard.h>
#include <types.h>
#include <irq.h>

/** BUILT-IN KEYMAP **/
static uint16 kb_default_keymap[] = 
  {
    NULL,
    KEY_ESC,
    '1' , '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    KEY_ENTER,
    KEY_LCONTROL,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
    '`',
    KEY_LSHIFT,
    '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
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
    // ENOUGHT, BUT THERE ARE MORE
    };

static struct
{
  uint8 shift, caps;
  uint8 numlock;
}kb_flags = {0, 0, 0};

/** Keyboard buffer **/
static uint16 kb_key = 0x00;

/** METHODS **/
void kb_handler(struct regs *regs)
{
  uint8  c;

  c = inportb(0x60);

  switch(c)
  {
  case 0x2A:
    kb_flags.shift = 1;
    break;
  case 0xAA:
    kb_flags.shift = 0;
    break;
  default:
    if(c & 0x80)
    {
      // Breakcode
    }else
    {
      kb_key = kb_default_keymap[c];

      if(kb_key > 'a' && kb_key < 'z')
      {
	if(kb_flags.shift && !kb_flags.caps)
	  kb_key = kb_key - 'a' + 'A';
      }
    }
  }
}

/* kb_init()
 * Install handler
 */
void kb_init()
{
  irq_install_handler(1, kb_handler);
}

/* kb_get()
 * Returns the last key
 */
uint16 kb_getc()
{
  static uint16 aux;
  // Wait until we have something
  while(!kb_key)
    asm("hlt");
  // Pop from buffer
  aux = kb_key;
  kb_key = 0x00;
  // Return
  return aux;
}
