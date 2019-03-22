#include "keyboard_ext.h"
#include "interrupts.h"
#include "gl.h"
#include "malloc.h"
#include "gpio.h"
#include "save.h"

#define MAX_PICS 24


// NOTE TO SELF -- HOW TO INTEGRATE EVERYTHING
// 1. Keep current display state global/public
// 2. Do thermal calculation every iteration
// 3. After calculation, do switch statement on display state and call appropriate function

int pics_saved = 0;
color_t*[MAX_PICS] pic_arr;

int curr_display_state = STREAM;

int prev_display_state = STREAM;
int gallery_index = 0;

int get_disp_state(void) {
	return curr_display_state;
}

void save_init(void) {

    //Use pin 17 instead of default (pin 3) because that is used by thermal camera
	keyboard_init_ext(GPIO_PIN17, KEYBOARD_DATA);
    //pic_arr = (color_t **)malloc(1);
}

void take_pic(void) {
	int bytes = 4 * 32 * 24;
	color_t* curr_pic = malloc(bytes);

	int height = gl_get_height();
	int width = gl_get_width();

	for (int row = 0; row < height; row++) { // each row -- y
		for (int col = 0; col < width; col++) { // each column -- x
			color_t color = gl_read_pixel(col, row);
			curr_pic[width*row + col] = color;
		}
	}

	pics_saved++;
	//pic_arr = realloc((void *)pic_arr, pics_saved);
	pic_arr[pics_saved - 1] = curr_pic;
	
	// read pixel colors from image
	// store in 768-4byte array
	// store ptr
}

void disp_pic(void) {
	gl_clear(GL_BLACK);
	color_t* curr_pic = pic_arr[gallery_index];

	int height = gl_get_height();
	int width = gl_get_width();


	for (int row = 0; row < height; row++) { // each row -- y
		for (int col = 0; col < width; col++) { // each column -- x
			color_t color = curr_pic[width*row + col];
			gl_draw_pixel(col, row, color);
		}
	}
	gl_swap_buffer();
}

void disp_help_screen(void) {
	gl_clear(GL_BLACK);
	gl_draw_string(0, 0, "HELP MENU:", GL_WHITE);
	gl_draw_string(0, gl_get_char_height(), "[e] - enter/exit Gallery", GL_WHITE);
	gl_draw_string(0, gl_get_char_height() * 2, "[Space] - take picture", GL_WHITE);
	gl_draw_string(0, gl_get_char_height() * 3, "[l] - next picture", GL_WHITE);
	gl_draw_string(0, gl_get_char_height() * 4, "[j] - previous picture:", GL_WHITE);
}

void interpret_user_keys(char key) {
	// char key = keyboard_read_next_ext();

	switch(curr_display_state) {
		case STREAM:
			switch(key) {
				case ' ':
					take_pic();
					break;
				case 'e':
					if (pics_saved > 0) {
						curr_display_state = GALLERY;
					}
					break;
				case 'h':
					curr_display_state = HELP;
					prev_display_state = STREAM;
					break;
			}
			break;
		case GALLERY:
			switch(key) {
				case 'l': // go forward in pics, wrap around -- GALLERY
					gallery_index++;
					if (gallery_index >= pics_saved) {
						gallery_index = 0;
					}
					break;
				case 'j': // go backward in pics, wrap around -- GALLERY
					gallery_index--;
					if (gallery_index < 0) {
						gallery_index = pics_saved - 1;
					}
					break;
				case 'e':
					curr_display_state = STREAM;
					break;
				case 'h':
					curr_display_state = HELP;
					prev_display_state = GALLERY;
					break;
			}
			break;
		case HELP:
			switch(key) {
				case 'h':
					curr_display_state = prev_display_state;
					break;
			}
			break;
	}
}

