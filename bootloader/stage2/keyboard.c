/*
 * keyboard.h
 *
 * Keyboard handling
 */
#include <keyboard.h>
#include <types.h>
#include <irq.h>
#include <ports.h>
#include <printf.h>

/* Types */
static struct {
    uint8_t shift, caps;
    uint8_t numlock;
    uint8_t lcontrol, lalt;
    uint8_t rcontrol, ralt;
    uint8_t escaped;
} kb_flags = { 0, 0, 0, 0, 0, 0, 0, 0 };

/* Keymap */
extern uint16_t kb_keymap[], kb_shift_keymap[], kb_altgr_keymap[], kb_escaped_keymap[];

/** Keyboard buffer **/
static struct {
    uint16_t key[KB_BUFFER_LEN];
    uint8_t pop_index, push_index;
} kb_buffer;

void kb_handler(Registers *regs)
{
    static uint8_t c;
    static uint16_t key;
    static uint8_t keyDown;

    // Get byte sent
    c = inportb(0x60);

    // Is the escape code?
    if (c == 0xE0)
        kb_flags.escaped = 1;
    else {
        // ¿Up?
        keyDown = !(c & 0x80);

        // Ignore break code
        c = c & 0x7F;

        // Get key from keymap
        if (kb_flags.escaped)
            key = kb_escaped_keymap[c];
        else if (kb_flags.shift)
            key = kb_shift_keymap[c];
        else if (kb_flags.ralt)
            key = kb_altgr_keymap[c];
        else
            key = kb_keymap[c];

        // Do something
        switch (key) {
        case KEY_LSHIFT:
        case KEY_RSHIFT:
            kb_flags.shift = keyDown;
            break;
        case KEY_LCONTROL:
            kb_flags.lcontrol = keyDown;
            break;
        case KEY_RCONTROL:
            kb_flags.rcontrol = keyDown;
            break;
        case KEY_LALT:
            kb_flags.lalt = keyDown;
            break;
        case KEY_RALT:
            kb_flags.ralt = keyDown;
            break;
        case KEY_CAPS:
            kb_flags.caps = keyDown ^ kb_flags.caps;
            break;
        default:
            if (keyDown) // Get only the key when it's down
            {
                if (key) {
                    // Shift block
                    if (kb_flags.caps) {
                        if (key >= 'a' && key <= 'z')
                            key = key - 'a' + 'A';
                        else if (key >= 'A' && key <= 'Z')
                            key = key - 'A' + 'a';
                    }

                    // Push into buffer if it isn't full
                    if ((kb_buffer.push_index + 1) % KB_BUFFER_LEN
                            != kb_buffer.pop_index) {
                        kb_buffer.key[kb_buffer.push_index] = key;
                        kb_buffer.push_index = (kb_buffer.push_index + 1)
                                % KB_BUFFER_LEN;
                    }
                }
                else {
                    // Unknown code
                    printf(
                            "[kb_handler()] Unknown scancode: 0x%x (Escaped: %i)\n",
                            c, kb_flags.escaped);
                }
            } // if keyDown
        } // switch

        // Reset escaped
        kb_flags.escaped = 0;

    } // else c == 0xE0
}

/**
 * Install handler
 */
void kb_init()
{
    irq_install_handler(KB_IRQ, kb_handler);
    // Buffer
    kb_buffer.push_index = kb_buffer.pop_index = 0;
}

/**
 * Returns the last key
 */
uint16_t kb_getc()
{
    static uint8_t index;

    // Wait until we have something
    while (kb_buffer.push_index == kb_buffer.pop_index)
        asm("hlt");

    // Pop from buffer
    index = kb_buffer.pop_index;
    kb_buffer.pop_index = (kb_buffer.pop_index + 1) % KB_BUFFER_LEN;

    // Return
    return kb_buffer.key[index];
}
