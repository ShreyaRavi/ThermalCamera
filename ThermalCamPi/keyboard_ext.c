#include "gpio.h"
#include "gpioextra.h"
#include "keyboard_ext.h"
#include "ps2.h"
#include "timer.h"
#include "printf.h"
#include "interrupts.h"
#include "ringbuffer.h"


static unsigned int CLK, DATA; 

const unsigned char ERR_CODE = 0x7e;

// global to keep track of state of modifiers
static unsigned int modifiers = 0;

static unsigned int code = 0;
static unsigned int sum_bits = 0;
static unsigned int bit_num = 0;


// keep track of 2 types of modifiers
typedef enum {
    TOGGLE, // CAPS_LOCK, NUM_LOCK, SCROLL_LOCK
    HOLD // SHIFT, ALT, CTRL
} key_type;

// interrupt handler -- called when clock falling edge detected
static bool bit_received(unsigned int pc) 
{
    // clear event on clock line so that event isn't forever triggered
    if (gpio_check_and_clear_event(CLK)) {
        // keep reading bits until valid scancode is read

        // read current bit
        unsigned int bit = gpio_read(DATA);

        if (bit_num == 0) { // start bit -- must be 0; if not, reset
            if (bit != 0) {
                // reset bit_num to 0, don't increment
                bit_num = 0;
            } else {
                code = 0;
                sum_bits = 0;
                bit_num++;
            }
        } else if (bit_num == 9) { // parity bit
            if (bit == (sum_bits % 2)) { // parity not correct
                // reset bit_num to 0, don't increment
                bit_num = 0;
            } else {
                bit_num++;
            }
        } else if (bit_num == 10) { // stop bit
            if (bit == 1) { // stop bit must be 1 
                rb_enqueue(rb_scancode, code);
            }
            bit_num = 0;
        } else {
            // process data bits and update globals accordingly
            sum_bits += bit; 
            code |= bit << (bit_num - 1);
            bit_num++;
        }

        return true;
    }
    return false;
}


// set up interrupt on clock falling edge
void setup_interrupts(void) 
{
    gpio_enable_event_detection(CLK, GPIO_DETECT_FALLING_EDGE);
    interrupts_attach_handler(bit_received);
    interrupts_enable_source(INTERRUPTS_GPIO3);
    interrupts_global_enable();
}

// updates global modifiers given a key action and type of key
void update_modifiers(key_action_t action, key_type type, keyboard_modifiers_t modifier) {
    switch (type) {
        case TOGGLE:
            // toggle only affects bits on release because release only happens once
            // but press can happen multiple times (if key held down)
            if (action.what == KEY_RELEASE) {
                if ((modifiers & modifier) == 0) { // if bit not already set to 1
                    modifiers |= modifier; // set bit to 1
                } else {
                    modifiers &= ~modifier; // set bit to 0
                }
            }
            break;
        case HOLD:
            if (action.what == KEY_PRESS) { // if key is being pressed
                modifiers |= modifier; // set bit to 1
            } else { // if key being released
                modifiers &= ~modifier; // set bit to 0
            }
            break;
    }
}

// uses polling to get falling clock edge -- no longer used
void wait_for_falling_clock_edge(void) {
    while (gpio_read(CLK) == 0) {}
    while (gpio_read(CLK) == 1) {}
}

void keyboard_init_ext(unsigned int clock_gpio, unsigned int data_gpio) 
{
    CLK = clock_gpio;
    gpio_set_input(CLK); 
    gpio_set_pullup(CLK); 
 
    DATA = data_gpio;
    gpio_set_input(DATA); 
    gpio_set_pullup(DATA); 

    // initialize settings for interrupts
    setup_interrupts();

    // get space for ring buffer and pointer to buffer
    rb_scancode = rb_new();

}

unsigned char keyboard_read_scancode_ext(void) 
{
    int scancode = 0;
    // wait until a scancode is successfully dequeued
    while (!rb_dequeue(rb_scancode, &scancode));
    // return dequeued scancode
    return scancode;
}

key_action_t keyboard_read_sequence_ext(void)
{
    key_action_t action;
    unsigned char code = keyboard_read_scancode_ext();
    if (code == PS2_CODE_EXTENDED) { // ignore the extended code
        code = keyboard_read_scancode_ext();
    }

    // if the code was a break code, read a new scancode -- break code = on release
    if (code == PS2_CODE_RELEASE) {
        action.keycode = keyboard_read_scancode_ext();
        action.what = KEY_RELEASE;
    } else { // if the code was not a break code, then it was the scancode -- on press
        action.keycode = code;
        action.what = KEY_PRESS;
    }
    
    return action;
}

key_event_t keyboard_read_event_ext(void) 
{    
    // create an event, set action
    key_event_t event;
    key_action_t action = keyboard_read_sequence_ext();
    event.action = action;

    // get key from ps2_keys lookup table
    event.key = ps2_keys[action.keycode];

    // use first entry in ps2_key_t struct to compare to key codes and update modifiers accordingly
    switch(event.key.ch) {
        case PS2_KEY_SHIFT:
            update_modifiers(action, HOLD, KEYBOARD_MOD_SHIFT);
            break;
        case PS2_KEY_ALT:
            update_modifiers(action, HOLD, KEYBOARD_MOD_ALT);
            break;
        case PS2_KEY_CTRL:
            update_modifiers(action, HOLD, KEYBOARD_MOD_CTRL);
            break;
        case PS2_KEY_CAPS_LOCK:
            update_modifiers(action, TOGGLE, KEYBOARD_MOD_CAPS_LOCK);
            break;
        case PS2_KEY_NUM_LOCK:
            update_modifiers(action, TOGGLE, KEYBOARD_MOD_NUM_LOCK);
            break;
        case PS2_KEY_SCROLL_LOCK:
            update_modifiers(action, TOGGLE, KEYBOARD_MOD_SCROLL_LOCK);
            break;
    }
    
    // set modifiers to newly updated modifiers
    event.modifiers = modifiers;
    
    return event;
}


unsigned char keyboard_read_next_ext(void) 
{
    // keep reading events until it is a press that is not a modifier
    key_event_t event;
    do {
        event = keyboard_read_event_ext();
    } while (event.action.what == KEY_RELEASE ||
     event.key.ch == PS2_KEY_SHIFT ||
      event.key.ch == PS2_KEY_CAPS_LOCK ||
      event.key.ch == PS2_KEY_ALT ||
      event.key.ch == PS2_KEY_CTRL ||
      event.key.ch == PS2_KEY_NUM_LOCK ||
      event.key.ch == PS2_KEY_SCROLL_LOCK);


    if ((event.modifiers & KEYBOARD_MOD_SHIFT) != 0) { // SHIFT is enabled
        // SHIFT only affects printable characters (<= 0x7f)
        if (event.key.ch <= 0x7f && event.key.other_ch != 0) {
            return event.key.other_ch;
        }
    }

    if ((event.modifiers & KEYBOARD_MOD_CAPS_LOCK) != 0) { // CAPS LOCK is enabled
        // only affects letters
        if (event.key.ch >= 'a' && event.key.ch <= 'z') {
            return event.key.other_ch;
        }
    }

    return event.key.ch;
}

unsigned char keyboard_read_next_ext_custom(void) 
{
    // keep reading events until it is a release that is not a modifier
    key_event_t event;
    do {
        event = keyboard_read_event_ext();
    } while (event.action.what == KEY_PRESS ||
     event.key.ch == PS2_KEY_SHIFT ||
      event.key.ch == PS2_KEY_CAPS_LOCK ||
      event.key.ch == PS2_KEY_ALT ||
      event.key.ch == PS2_KEY_CTRL ||
      event.key.ch == PS2_KEY_NUM_LOCK ||
      event.key.ch == PS2_KEY_SCROLL_LOCK);


    if ((event.modifiers & KEYBOARD_MOD_SHIFT) != 0) { // SHIFT is enabled
        // SHIFT only affects printable characters (<= 0x7f)
        if (event.key.ch <= 0x7f && event.key.other_ch != 0) {
            return event.key.other_ch;
        }
    }

    if ((event.modifiers & KEYBOARD_MOD_CAPS_LOCK) != 0) { // CAPS LOCK is enabled
        // only affects letters
        if (event.key.ch >= 'a' && event.key.ch <= 'z') {
            return event.key.other_ch;
        }
    }

    return event.key.ch;
}
