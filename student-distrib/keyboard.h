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

/* Externally-visible functions */

/* Initialize keyboard */
void keyboard_init(void);

void process_keypress(void);

void determine_case(unsigned char char_pressed);

#endif /* _KEYBOARD_H */
