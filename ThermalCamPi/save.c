#include "keyboard_ext.h"
#include "interrupts.h"
#include "gl.h"
#include "malloc.h"
#include "gpio.h"
#include "save.h"

#define MAX_PICS 24

int pics_saved = 0;
color_t* pic_arr[MAX_PICS];

int curr_display_state = STREAM;

int gallery_index = 0;

/*
* Get current display state to determine what to display
*/
int get_disp_state(void) {
	return curr_display_state;
}

/*
 * Initialize the save functions by allowing keyboard input (with interrupts enabled)
 */
void save_init(void) {
    //Use pin 17 instead of default (pin 3) because that is used by thermal camera
	keyboard_init_ext(GPIO_PIN17, KEYBOARD_DATA);
}

/*
 * Takes a picture of the current stream and store for later viewing
 */
void take_pic(void) {
	// create enough space to store full image data
	int bytes = 4 * 32 * 24;
	color_t* curr_pic = malloc(bytes);

	int height = gl_get_height();
	int width = gl_get_width();

	// read pixel colors from image
	for (int row = 0; row < height; row++) { // each row -- y
		for (int col = 0; col < width; col++) { // each column -- x
			color_t color = gl_read_pixel(col, row);
			// store in 768-4byte array
			curr_pic[width*row + col] = color;
		}
	}

	// store pic in array
	pics_saved++;
	pic_arr[pics_saved - 1] = curr_pic;
	
}

/*
 * Display picture in photo gallery at current index
 *
 * Index stored globally and determined by user with use 
 * of [j] and [l] keys while in Gallery mode.
 */
void disp_pic(void) {

	gl_clear(GL_BLACK);

	// get current picture using globally stored index
	color_t* curr_pic = pic_arr[gallery_index];

	int height = gl_get_height();
	int width = gl_get_width();

	// draw image on screen from stored picture
	for (int row = 0; row < height; row++) { // each row -- y
		for (int col = 0; col < width; col++) { // each column -- x
			color_t color = curr_pic[width*row + col];
			gl_draw_pixel(col, row, color);
		}
	}

	// display image
	gl_swap_buffer();
}


/*
 * State machine that takes in user's key press and accordingly
 * affects globals or state
 * 
 * State determined by this state machine is used by caller function to 
 * determine which image is displayed.
 */
void interpret_user_keys(char key) {

	switch(curr_display_state) {
		// STREAM state: can press [space] to take pic, or press [e] to enter into gallery mode
		case STREAM:
			switch(key) {
				case ' ':
					// don't take picture if max number of pictures already saved
					if (pics_saved < MAX_PICS) {
						take_pic();
					}
					break;
					// enter into gallery mode if there are any pictures to be viewed
				case 'e':
					if (pics_saved > 0) {
						curr_display_state = GALLERY;
					}
					break;
			}
			break;
		// GALLERY: [l] to go forward, [j] to go back, [e] to enter back into STREAM state
		case GALLERY:
			switch(key) {
				case 'l': // go forward in pics, wrap around
					gallery_index++;
					if (gallery_index >= pics_saved) {
						gallery_index = 0;
					}
					break;
				case 'j': // go backward in pics, wrap around 
					gallery_index--;
					if (gallery_index < 0) {
						gallery_index = pics_saved - 1;
					}
					break;
				case 'e': // enter back into stream mode 
					curr_display_state = STREAM;
					break;
			}
			break;
	}
}

