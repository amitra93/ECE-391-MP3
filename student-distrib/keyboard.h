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


/* Special keys */
#define LSHIFT_PRESS	0x2A
#define RSHIFT_PRESS	0x36
#define	CAPS_PRESS		0x3A
#define LSHIFT_RELEASE	0xAA
#define RSHIFT_RELEASE	0xB6


// global variable to select between uppercase and lowercase letters
char* array_to_use;

// determine status of whether SHIFT/CAPS is pressed or not
int is_uppercase;

int is_control_pressed;

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
void determine_case(unsigned char char_pressed);

void change_keys_pressed(unsigned char char_pressed, unsigned char char_to_print);


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


#endif /* _KEYBOARD_H */
