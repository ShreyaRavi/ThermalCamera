#include "keyboard_ext.h"
#include "interrupts.h"
#include "gl.h"
#include "malloc.h"
#include "gpio.h"
#include "save.h"


// NOTE TO SELF -- HOW TO INTEGRATE EVERYTHING
// 1. Keep current display state global/public
// 2. Do thermal calculation every iteration
// 3. After calculation, do switch statement on display state and call appropriate function

int pics_saved = 0;
color_t** pic_arr;

int prev_display_state = STREAM;
int display_state = STREAM;
int gallery_index = 0;

void save_init(void) {

    //Use pin 17 instead of default (pin 3) because that is used by thermal camera
	keyboard_init_ext(GPIO_PIN17, KEYBOARD_DATA);
    pic_arr = (color_t **)malloc(1);
}

void take_pic(void) {
	int bytes = 4 * 32 * 24;
	color_t* curr_pic = malloc(bytes);

	int h = gl_get_height();
	int w = gl_get_width();

	for (int row = 0; row < h; row++) { // each row -- y
		for (int col = 0; col < w; col++) { // each column -- x
			color_t color = gl_read_pixel(col, row);
			curr_pic[gl_get_height()*row + col] = color;
		}
	}

	pics_saved++;
	pic_arr = realloc((void *)pic_arr, pics_saved);
	pic_arr[pics_saved - 1] = curr_pic;
	
	// read pixel colors from image
	// store in 768-4byte array
	// store ptr
}

void disp_pic(void) {
	gl_clear(GL_BLACK);
	color_t* curr_pic = pic_arr[gallery_index];
	for (int row = 0; row < gl_get_height(); row++) { // each row -- y
		for (int col = 0; col < gl_get_width(); col++) { // each column -- x
			color_t color = curr_pic[gl_get_height()*row + col];
			gl_draw_pixel(col, row, color);
		}
	}
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

	switch(display_state) {
		case STREAM:
			switch(key) {
				case ' ':
					take_pic();
					break;
				case 'e':
					if (pics_saved > 0) {
						display_state = GALLERY;
					}
					break;
				case 'h':
					display_state = HELP;
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
					display_state = STREAM;
					break;
				case 'h':
					display_state = HELP;
					prev_display_state = GALLERY;
					break;
			}
			break;
		case HELP:
			switch(key) {
				case 'h':
					display_state = prev_display_state;
					break;
			}
			break;
	}
}

