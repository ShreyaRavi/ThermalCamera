#ifndef SAVE_H
#define SAVE_H

enum DISP_STATE {
	STREAM,
	GALLERY,
	HELP
};


void save_init(void);
void interpret_user_keys(char key);
void disp_pic(void);
void disp_help_screen(void);


#endif