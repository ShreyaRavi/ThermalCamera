#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "gpio.h"
#include "ps2.h"
#include "ringbuffer.h"

/*
 * Module to read keys typed on a PS/2 keyboard.
 *
 * You implement this module in assignments 5 and 7.
 *
 * Author: Philip Levis <pal@cs.stanford.edu>
 * Author: Julie Zelenski <zelenski@cs.stanford.edu>
 *
 * Last updated:   February 2019
 */


typedef struct {
    enum { KEY_PRESS, KEY_RELEASE } what;
    unsigned char keycode;
} key_action_t;

/*
 * These bit flags are used for the state of the various modifier
 * keys on the keyboard.
 */
typedef enum {
    KEYBOARD_MOD_SCROLL_LOCK = 1 << 0,
    KEYBOARD_MOD_NUM_LOCK = 1 << 1,
    KEYBOARD_MOD_CAPS_LOCK = 1 << 2,
    KEYBOARD_MOD_SHIFT = 1 << 3,
    KEYBOARD_MOD_ALT = 1 << 4,
    KEYBOARD_MOD_CTRL = 1 << 5,
} keyboard_modifiers_t;

typedef struct {
    key_action_t action;                // see struct declared above
    ps2_key_t key;                      // entry taken from ps2_keys table (see ps2.h)
    keyboard_modifiers_t modifiers;     // modifiers in effect, composed of above bit flags
} key_event_t;


#define KEYBOARD_CLOCK GPIO_PIN3
#define KEYBOARD_DATA GPIO_PIN4

static rb_t *rb;

/*
 * `keyboard_init`: Required initialization for keyboard
 *
 * The keyboard must first be initialized before any key events can be read.
 * The first and second arguments identify which GPIO pins to use for the
 * PS/2 clock and data lines, respectively.
 *
 * Although `keyboard_init` configures the requested clock and data GPIOs
 * to use the internal pull-up resistors, it is recommended to choose GPIO
 * pins whose default state is already pull-up, such as KEYBOARD_CLOCK and
 * KEYBOARD_DATA defined above. This avoid timing issues due to the keyboard
 * attempting to handshake with the Pi before `keyboard_init` has executed.
 */
void keyboard_init_ext(unsigned int clock_gpio, unsigned int data_gpio);


/*
 * `keyboard_read_next`: Top level keyboard interface.
 *
 * This function reads (blocking) the next key typed on the keyboard.
 * The character returned reflects the current keyboard modifier settings
 * for shift and caps lock.
 *
 * Characters returned that have value <= 0x7f '~' are printable Ascii
 * characters. Character values >= 0x90 are returned for those keys that are
 * are not associated with an Ascii character (e.g. arrow and function keys).
 * See the ps2_codes defined in ps2.h for constants used for those keys.
 * This function calls `keyboard_read_event`.
 */
unsigned char keyboard_read_next_ext(void);


/*
 * `keyboard_read_event`: Mid level keyboard interface.
 *
 * The function reads (blocking) the next key event from the keyboard.
 * Returns a `key_event_t` struct that represents the key event.  The
 * struct includes the key that was pressed or released and the state
 * of the modifier flags. If this event is a press or release of a
 * modifier key (CTRL, ALT, SHIFT, etc.), the modifiers field in the
 * returned event show the updated state of the modifiers after
 * incorporating this key event.
 * This function calls `keyboard_read_sequence` to read a scancode sequence.
 */
key_event_t keyboard_read_event_ext(void);


/*
 * `keyboard_read_sequence`: Low level keyboard interface.
 *
 * Reads a sequence of scancode bytes corresponding to the press or
 * release of a single key.  Returns a `key_action_t` struct that
 * represents the key action for the sequence read. Reads 1, 2, or 3
 * scancodes:
 *    1 byte:  ordinary key press
 *    2 bytes: ordinary key release or extended key press
 *    3 bytes: extended key release
 * This function calls `keyboard_read_scancode` to read each scancode.
 */
key_action_t keyboard_read_sequence_ext(void);


/*
 * `keyboard_read_scancode`: Bottom level keyboard interface.
 *
 * Read (blocking) a single scancode from the PS/2 keyboard.
 * Bits are read on the falling edge of the clock.
 *
 * Tries to read 11 bits: 1 start bit, 8 data bits, 1 parity bit, and 1 stop bit
 *
 * Restarts the scancode if:
 *   (lab5) The start bit is incorrect
 *   (assign5) or if parity or stop bit is incorrect
 *
 * Returns the 8 data bits of a well-formed PS/2 scancode.
 * Will not return until it reads a complete and valid scancode.
 */
unsigned char keyboard_read_scancode_ext(void);


/*
 * `keyboard_use_interrupts`
 *
 * Change keyboard from default polling behavior to instead configure interrupts
 * for gpio events. After setting keyboard to use interrupts, client must
 * also globally enable interrupts at system level.
 */
void keyboard_use_interrupts(void);


#endif