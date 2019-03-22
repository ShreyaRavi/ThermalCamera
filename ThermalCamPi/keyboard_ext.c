#include "gpio.h"
#include "gpioextra.h"
#include "keyboard_ext.h"
#include "interrupts.h"
#include "ps2.h"
#include "assert.h"
#include "ringbuffer.h"

static unsigned int CLK, DATA; 

volatile static unsigned int interrupt_cnt = 0;  
volatile static keyboard_modifiers_t mod = 0;
static unsigned char code = 0;
static unsigned char parity_count = 0;
static unsigned char stop_bit = 0;
static unsigned char scancode = 0;

unsigned char get_scancode(void) {
    unsigned int temp = 0;
    if (scancode != 0) {
        temp = scancode;
        scancode = 0;
    }
    return temp;
}

void wait_for_falling_clock_edge(void) {
    while (gpio_read(CLK) == 0) {}
    while (gpio_read(CLK) == 1) {}
}

static bool interrupt_test(unsigned int pc) {
    if (!gpio_check_event(CLK)) {
        return false;
    }

    interrupt_cnt++;

    unsigned char bit = gpio_read(DATA);

    switch (interrupt_cnt % 11) {
        case 1: //start bit
        code = 0;
        parity_count = 0;
        stop_bit = 0;
        break;

        case 10: //parity bit
        parity_count += bit;
        break;

        case 0: //stop bit
        stop_bit = bit;
        if (stop_bit && (parity_count & 1))
            rb_enqueue(rb, code);
        break;

        default: //data bits
        code |= bit << ((interrupt_cnt % 11) - 2);
        parity_count += bit;
        break;
    }

    gpio_clear_event(CLK);
    return true;
}

// void setup_interrupts(unsigned int *cnt) 
// {
    
//     interrupt_cnt = cnt;
// }

void keyboard_init_ext(unsigned int clock_gpio, unsigned int data_gpio) 
{

    printf("keyboard_init\n");
    CLK = clock_gpio;
    gpio_set_input(CLK); 
    gpio_set_pullup(CLK); 
 
    DATA = data_gpio;
    gpio_set_input(DATA); 
    gpio_set_pullup(DATA); 

    rb = rb_new();

    gpio_enable_event_detection(CLK, GPIO_DETECT_FALLING_EDGE);
    interrupts_attach_handler(interrupt_test);
    interrupts_enable_source(INTERRUPTS_GPIO3);
    interrupts_global_enable();
}

unsigned char read_bit(void) {
    wait_for_falling_clock_edge();
    return gpio_read(DATA);
}

unsigned char keyboard_read_scancode_ext(void) 
{
    int cur_scancode = 0;
    while (!rb_dequeue(rb, &cur_scancode)) {}
    return cur_scancode;
}

key_action_t keyboard_read_sequence_ext(void)
{
    key_action_t action;
    action.keycode = keyboard_read_scancode_ext();

    //extended codes
    if (action.keycode == PS2_CODE_EXTENDED) {
        action.keycode = keyboard_read_scancode_ext();
    }

    //release code
    if (action.keycode == PS2_CODE_RELEASE) {
        action.what = KEY_RELEASE;

        //get the actual key to release
        action.keycode = keyboard_read_scancode_ext();
    } else {
        action.what = KEY_PRESS;
    }

    return action;
}

key_event_t keyboard_read_event_ext(void) 
{
    key_event_t event;

    event.action = keyboard_read_sequence_ext();
    event.key = ps2_keys[event.action.keycode];

    switch (event.action.keycode) {

        case 0x7e:
        if (event.action.what == KEY_PRESS)
            mod ^= KEYBOARD_MOD_SCROLL_LOCK;
        break;

        case 0x77:
        if (event.action.what == KEY_PRESS)
            mod ^= KEYBOARD_MOD_NUM_LOCK;
        break;

        case 0x58:
        if (event.action.what == KEY_PRESS)
            mod ^= KEYBOARD_MOD_CAPS_LOCK;
        break;

        case 0x12:
        case 0x59:
        if (event.action.what == KEY_PRESS) {
            mod |= KEYBOARD_MOD_SHIFT;
        } else {
            mod &= ~KEYBOARD_MOD_SHIFT;
        }
        break;

        case 0x11:
        if (event.action.what == KEY_PRESS) {
            mod |= KEYBOARD_MOD_ALT;
        } else {
            mod &= ~KEYBOARD_MOD_ALT;
        }
        break;

        case 0x14:
        if (event.action.what == KEY_PRESS) {
            mod |= KEYBOARD_MOD_CTRL;
        } else {
            mod &= ~KEYBOARD_MOD_CTRL;
        }
        break;

        default:
        break;
    }

    event.modifiers = mod;

    return event;
}


unsigned char keyboard_read_next_ext(void) 
{
    unsigned char caps = 0;
    unsigned char shift = 0;

    while (1) {

        key_event_t event = keyboard_read_event_ext();

        while (event.action.what == KEY_RELEASE) {
            event = keyboard_read_event_ext();
        }

        if ((event.modifiers & KEYBOARD_MOD_CAPS_LOCK) >> 2) {
            caps = 1;
        }

        if ((event.modifiers & KEYBOARD_MOD_SHIFT) >> 3) {
            shift = 1;
        }

        unsigned char first = event.key.ch;

        //if it's a modifier that's not caps or shift, print out first
        if ((event.action.keycode == 0x7e || 
            event.action.keycode == 0x77 || 
            event.action.keycode == 0x11 || 
            event.action.keycode == 0x14)) {
            return first;
        }

        //if it's not caps or shift
        if (!(event.action.keycode == 0x58 || 
            event.action.keycode == 0x12 || 
            event.action.keycode == 0x59)) {

            if (first >= 'a' && first <= 'z') {
                if ((caps & 1) || (shift & 1)) {
                    return event.key.other_ch;
                } else {
                    return first;
                }
            } else {
                if (shift & 1) {
                    return event.key.other_ch;
                } else {
                    return event.key.ch;
                }
            }
        }
    }
}
