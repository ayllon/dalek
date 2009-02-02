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
  uint8 lcontrol, lalt;
  uint8 rcontrol, ralt;
  uint8 escaped;
}kb_flags = {0, 0, 0, 0, 0, 0, 0, 0};

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
    // Escaped
  case 0xE0:
    kb_flags.escaped = 1;
    break;
    // Mod keys
  case 0x2A: case 0x36: // Shift down
    kb_flags.shift = 1;
    break;
  case 0xAA: case 0xB6: // Shift up
    kb_flags.shift = 0;
    break;
  case 0x1D: // Control down
    if(kb_flags.escaped)
      kb_flags.rcontrol = 1;
    else
      kb_flags.lcontrol = 1;
    break;
  case 0x9D: // Control up
    if(kb_flags.escaped)
      kb_flags.rcontrol = 0;
    else
      kb_flags.lcontrol = 0;
    break;
  case 0x38: // Alt down
    if(kb_flags.escaped)
      kb_flags.ralt = 1;
    else
      kb_flags.lalt = 1;
    break;
 case 0xB8: // Alt up
    if(kb_flags.escaped)
      kb_flags.ralt = 0;
    else
      kb_flags.lalt = 0;
    break;
  case 0x3A: // Shift lock down
    kb_flags.caps = !kb_flags.caps;
    break;
    // Default
  default:
    if(c & 0x80)
    {
      // Breakcode (key up)
    }else
    {
      // Get char
      if(kb_flags.shift)
	key = kb_shift_keymap[c];
      else if(kb_flags.ralt)
	key = kb_altgr_keymap[c];
      else
	key = kb_keymap[c];

      // Check
      if(key)
      {
	// Shift block
	if(kb_flags.caps)
	{
	  if(key >= 'a' && key <= 'z')
	    key = key - 'a' + 'A';
	  else if(key >= 'A' && key <= 'Z')
	    key = key - 'A' + 'a';
	}

	// Push into buffer if it isn't full
	if((kb_buffer.push_index + 1) % KB_BUFFER_LEN != kb_buffer.pop_index)
	{
	  kb_buffer.key[kb_buffer.push_index] = key;
	  kb_buffer.push_index = (kb_buffer.push_index + 1) % KB_BUFFER_LEN;
	}
      }else
      {
	// Null char. Unknown
	printf("[kb_handler()] Unknown scancode: 0x%x\n", c);
      }
    }
  }
  // Reset escaped
  kb_flags.escaped = (c == 0xE0);
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
