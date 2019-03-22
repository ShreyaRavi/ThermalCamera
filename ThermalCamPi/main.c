#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "gl.h"
#include "math.h"
#include "colorize.h"
#include "keyboard_ext.h"
#include "thermal_lib.h"
#include "save.h"

static int test = 'a';

void test_send_single_char(void) {

    uart_init();

    timer_delay(7);

    while (1) {
        uart_putchar(test);

        test++;
        if (test > 'z') {
            test = 'a';
        }

        timer_delay_ms(100);

        // timer_delay_ms(1000);
        // uart_flush();
    }
}

void test_thermal_color(void) {
    thermal_init(0.0, 80.0, GRADIENT_IRON);

    float temp = 0.0;
    color_t color = 0;

    for (; temp <= 80.0; temp += 5.0) {
        color = get_thermal_color(temp);
        printf("Color: %x\n", color);
    }    
}

void test_get_key(void) {

    while (1) {
        // data_get();
        char c = keyboard_read_next_ext();
        printf("%c",c);
    }
}

void main(void) 
{
    uart_init();
    save_init();
    // thermal_init(20.0, 40.0, GRADIENT_IRON);
    data_get();
    // test_get_key();
    // data_transmit();
    // test_thermal_color();
    // test_send_single_char();
}

