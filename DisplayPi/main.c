#include "printf.h"
#include "uart.h"
#include "timer.h"
#include "math.h"
#include "gl.h"
#include "malloc.h"
#include "strings.h"
#include "keyboard.h"
#include "interrupts.h"

static const int sensor_disp_width = 32;
static const int sensor_disp_height = 24;
static int test = 'a';
static char *test_string;
static int curr_col = 0;
static int curr_row = 0;
static int num_cols = 0;
static int num_rows = 0;

void display_image(void) {
    for (int row = 0; row < sensor_disp_width; row++) { // row -- y value
        for (int col = 0; col < sensor_disp_height; col++) { // col -- x value
            while (!uart_haschar()) {

            }
            char b = uart_getchar();
            char g = uart_getchar();
            char r = uart_getchar();

            color_t color = gl_color(r, g, b);

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
    gl_draw_char(curr_col * gl_get_char_width(), curr_row * gl_get_char_height(), output, GL_WHITE);
    curr_col++;
    if (curr_col >= num_cols) {
        curr_row++;
        curr_col = 0;
    }
    if (curr_row >= num_rows) {
        gl_clear(GL_BLACK);
        curr_row = 0;
        curr_col = 0;
    }
    gl_swap_buffer();
}

void test_get_string(void) {
    test_string += uart_getchar();
    gl_draw_string(0, 0, test_string, GL_WHITE);
    gl_swap_buffer();

    if (strlen(test_string) > 20) {
        test_string = " ";
    }
}

void main(void) 
{
    uart_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    interrupts_global_enable();
    keyboard_use_interrupts();
    gl_init(sensor_disp_width, sensor_disp_height, GL_DOUBLEBUFFER);

    test_string = malloc(2000);
    num_cols = gl_get_width()/gl_get_char_width();
    num_rows = gl_get_height()/gl_get_char_height();

    timer_delay(7);

    while (1) {
        display_image();
        // test_disp();
        // test_get_single_char();
        //test_get_string();
    }
}

