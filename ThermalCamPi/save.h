#ifndef SAVE_H
#define SAVE_H

// enumeration to keep track of display state
enum DISP_STATE {
	STREAM,
	GALLERY
};

/*
 * Initialize the save functions by allowing keyboard input (with interrupts enabled)
 */
void save_init(void);

/*
 * State machine that takes in user's key press and accordingly
 * affects globals or state
 * 
 * State determined by this state machine is used by caller function to 
 * determine which image is displayed.
 */
void interpret_user_keys(char key);

/*
 * Display picture in photo gallery at current index
 *
 * Index stored globally and determined by user with use 
 * of [j] and [l] keys while in Gallery mode.
 */
void disp_pic(void);

/*
* Get current display state to determine what to display
*/
int get_disp_state(void);


#endif