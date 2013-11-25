/* keyboard.h - Keyboard handler (work in progress)
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
/* Constants */
#define DATA_PORT		0x60
#define STATUS_REG		0x64
#define CMD_REG			0x64


// ______________KEYBOARD CHARACTER STUFF______________


typedef struct key {
	unsigned char lowercase_char;
	unsigned char uppercase_char;
	unsigned char shift_char;
	unsigned char pressed;
	unsigned char to_display;
} key;



typedef struct key_orientation {
	key* keyboard_key;
	unsigned char orientation;
} key_orientation;


void init_keys();

unsigned char is_shift_pressed();

unsigned char is_caps_lock_pressed();

unsigned char is_control_pressed();

unsigned char is_alt_pressed();

unsigned char is_enter_pressed();

unsigned char is_pressed(key* keyboard_key);

unsigned char print_key(key* keyboard_key);

//____________END KEYBOARD CHARACTER STUFF_____________





// global variable to select between uppercase and lowercase letters
//char* array_to_use;

// determine status of whether SHIFT/CAPS is pressed or not
//int is_uppercase;

//int is_control_pressed;

volatile int shortcut_received;


//unsigned char keyboard_last_printable_key;

/* Externally-visible functions */

/* Helper function that determines whether to print uppercase or lowercase characters */

/*
 * void determine_case(unsigned char char_pressed)
 * DESCRIPTION: This is a helper function that determines whether to print uppercase or lowercase characters.
 *
 * INPUTS: char_pressed
 * OUTPUTS: none
 * SIDE EFFECTS: sets variable that determines whether to select from uppercase or lowercase array
 */
//void determine_case(unsigned char char_pressed);

/*
 * void change_keys_pressed(unsigned char char_pressed, unsigned char char_to_print);
 * DESCRIPTION: This is a helper function that determines whether to print uppercase or lowercase characters.
 *
 * INPUTS: char_pressed
 * OUTPUTS: none
 * SIDE EFFECTS: sets variable that determines whether to select from uppercase or lowercase array
 */
//void change_keys_pressed(unsigned char char_pressed, unsigned char char_to_print);


/*
 * void keyboard_init(void)
 * DESCRIPTION: This function initializes relevant global variables that we use when keyboard interrupts are generated
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: initializes global variables
 */
void keyboard_init(void);

/* Function that gets called whenever a keyboard interrupt is generated */

/*
 * void process_keypress(void)
 * DESCRIPTION: This is an internal helper function that gets called whenever we get a keyborad interrupt
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: sends relevant keypresses to the console
 */
void process_keypress(void);

void process_shortcuts(void);

unsigned char keyboard_wait_for_new_line(int max_chars);

#endif /* _KEYBOARD_H */
