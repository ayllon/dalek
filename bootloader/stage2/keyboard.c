/*
 * keyboard.h
 *
 * Keyboard handling
 *
 */
#include <keyboard.h>
#include <types.h>
#include <irq.h>
#include <ports.h>

/* Types */
static struct
{
  uint8 shift, caps;
  uint8 numlock;
}kb_flags = {0, 0, 0};

/* Keymap */
extern uint16 kb_keymap[], kb_shift_keymap[], kb_altgr_keymap[];

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
      // Get char
      if(kb_flags.shift)
	kb_key = kb_shift_keymap[c];
      else
	kb_key = kb_keymap[c];

      // Shift block
      if(kb_flags.caps)
      {
	if(kb_key > 'a' && kb_key < 'z')
	  kb_key = kb_key - 'a' + 'A';
	else if(kb_key > 'A' && kb_key < 'Z')
	  kb_key = kb_key - 'Z' + 'a';
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
