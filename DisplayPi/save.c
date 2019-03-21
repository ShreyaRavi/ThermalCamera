#include "keyboard.h"

void save_init(void) {
    interrupts_global_enable();
	keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    keyboard_enable_interrputs();
}

