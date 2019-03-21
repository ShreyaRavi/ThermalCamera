#include "keyboard.h"


int pics_saved = 0;

enum DISP_STATE {
	STREAM,
	GALLERY,
	HELP
};

DISP_STATE display_state = STREAM;
DISP_STATE prev_display_state = STREAM;

void save_init(void) {
    interrupts_global_enable();
	keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    keyboard_enable_interrputs();
}

void take_pic(void) {
	// read pixel colors from image
	// store in 768-4byte array
	// store ptr
}

void disp_pic(int index) {

}

void disp_help_screen(void) {

}

void interpret_user_keys(void) {
	char key = keyboard_read_next();
	int index = 0;

	switch(display_state) {
		case STREAM:
			switch(key) {
				case ' ':
					take_pic();
					break;
				case 'e':
					display_state = GALLERY;
					break;
				case 'h':
					display_state = HELP;
					break;
			}
			break;
		case GALLERY:
			switch(key) {
				case 'l': // go forward in pics, wrap around -- GALLERY
					index++;
					if (index >= pics_saved) {
						index = 0;
					}
					disp_pic(index);
					break;
				case 'j': // go backward in pics, wrap around -- GALLERY
					index--;
					if (index <= pics_saved) {
						index = pics_saved - 1;
					}
					disp_pic(index);
					break;
				case 'e':
					display_state = STREAM;
					break;
				case 'h':
					display_state = HELP;
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

	// switch(key) {
	// 	case ' ': // take pic and save -- STREAM
	// 		take_pic();
	// 		break;
	// 	case 'l': // go forward in pics, wrap around? -- GALLERY
	// 		index++;
	// 		if (index >= pics_saved) {
	// 			index = 0;
	// 		}
	// 		disp_pic(index);
	// 		break;
	// 	case 'j': // go backward in pics, wrap around? -- GALLERY
	// 		index--;
	// 		if (index <= pics_saved) {
	// 			index = pics_saved - 1;
	// 		}
	// 		disp_pic(index);
	// 		break;
	// 	case 'e': // [e]nter or [e]xit picture viewer -- BOTH STREAM AND GALLERY
	// 		switch(display_state) {
	// 			case STREAM:
	// 				display_state = GALLERY;
	// 				break;
	// 			case GALLERY:
	// 				display_state = STREAM;
	// 				break;
	// 		}
	// 		break;
	// 	case 'h': // help screen -- NEITHER
	// 		break;
	// }
}

