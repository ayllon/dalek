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
static struct
{
  uint16 key[KB_BUFFER_LEN];
  uint8 pop_index, push_index;
}kb_buffer;

/** METHODS **/
void kb_handler(struct regs *regs)
{
  uint8  c;
  uint16 key;

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
	key = kb_shift_keymap[c];
      else
	key = kb_keymap[c];

      // Shift block
      if(kb_flags.caps)
      {
	if(key > 'a' && key < 'z')
	  key = key - 'a' + 'A';
	else if(key > 'A' && key < 'Z')
	  key = key - 'Z' + 'a';
      }

      // Push into buffer
      if((kb_buffer.push_index + 1) % KB_BUFFER_LEN != kb_buffer.pop_index)
      {
	kb_buffer.key[kb_buffer.push_index] = key;
	kb_buffer.push_index = (kb_buffer.push_index + 1) % KB_BUFFER_LEN;
      }else
      {
	// Buffer full. Ignore character :-(
	return;
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
  // Buffer
  kb_buffer.push_index = kb_buffer.pop_index = 0;
}

/* kb_get()
 * Returns the last key
 */
uint16 kb_getc()
{
  static uint8 index;

  // Wait until we have something
  while(kb_buffer.push_index == kb_buffer.pop_index)
    asm("hlt");

  // Pop from buffer
  index = kb_buffer.pop_index;
  kb_buffer.pop_index = (kb_buffer.pop_index + 1) % KB_BUFFER_LEN;

  // Return
  return kb_buffer.key[index];
}
