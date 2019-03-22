#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "gl.h"
#include "math.h"
#include "colorize.h"
#include "keyboard_ext.h"
#include "thermal_lib.h"
#include "save.h"


/*
* Testing of wireless data transmission testing with ESP32s
*/
void test_send_single_char(void) {

    uart_init();

    timer_delay(7);
    static int test = 'a';

    while (1) {
        uart_putchar(test);

        test++;
        if (test > 'z') {
            test = 'a';
        }

        timer_delay_ms(100);

    }
}

/*
* Testing of calculation between temperature and thermal color
*/
void test_thermal_color(void) {
    thermal_init(0.0, 80.0, GRADIENT_IRON);

    float temp = 0.0;
    color_t color = 0;

    for (; temp <= 80.0; temp += 5.0) {
        color = get_thermal_color(temp);
        printf("Color: %x\n", color);
    }    
}

/*
* Testing modified keyboard read (reads on release as opposed to press)
*/
void test_get_key(void) {

    while (1) {
        char c = keyboard_read_next_ext();
        printf("%c",c);
    }
}

// if there is ever a keyboard input, interpret the key and change state as such
void interpret_any_keyboard_input(void) {
    // interrupts make sure that ringbuffer is always up-to-date
    // if it is not empty, a key has been pressed
    // only then, wait for the whole scancode to come in (using keyboard_read_next_ext)
    // this allows program to avoid stalling
    if (!rb_empty(rb_scancode)) {
        // wait for key release and read that key
        char c = keyboard_read_next_ext();

        // only interpret relevant keys
        if (c == 'e' ||  // [e]nter or [e]xit gallery mode
            c == 'l' ||  // [l] to go forward in pics
            c == 'j' ||  // [j] to go backward in pics
            c == ' ') { // [space] to take pic
            interpret_user_keys(c);
        }

    }
}

// initialize all independent modules (save, uart, thermal cam)
void init_modules() {
    uart_init();
    save_init();
    setup_data_read(20.0f, 40.0f);
}

void main(void) {
    // all initializations
    init_modules();
    while (1) {
        // read image and display to screen depending on state
        get_and_display_image();
        // make appropriate changes to display state and other data
        interpret_any_keyboard_input();
    }

}

