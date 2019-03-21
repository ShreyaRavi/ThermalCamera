#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "math.h"
#include "gl.h"

static const int sensor_disp_width = 32;
static const int sensor_disp_height = 24;
static int test = 'a';

void display_image(void) {
    for (int row = 0; row < sensor_disp_width; row++) { // row -- y value
        for (int col = 0; col < sensor_disp_height; col++) { // col -- x value
            while (!uart_haschar()) {

            }
            color_t color = uart_getchar();
            gl_draw_pixel(col, row, color);
        }
    }
    gl_swap_buffer();
}

void test_disp(void) {
    gl_clear(GL_BLACK);
    gl_draw_char(0, 0, test, GL_WHITE);
    gl_swap_buffer();

    test++;
    if (test > 'z') {
        test = 'a';
    }

    timer_delay_ms(100);
}

void test_get_single_char(void) {
    int output = uart_getchar();
    gl_draw_char(0, 0, output, GL_WHITE);
    gl_swap_buffer();
}

void main(void) 
{
    uart_init();
    gl_init(sensor_disp_width, sensor_disp_height, GL_DOUBLEBUFFER);

    timer_delay(7);

    while (1) {
        // display_image();
        // test_disp();
        test_get_single_char();
    }
}

