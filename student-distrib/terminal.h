/* terminal.h - Terminal driver (work in progress)
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"


/* Constants and variables */
#define BUFFER_SIZE		20


char input[BUFFER_SIZE];
int buffer_pointer;
int old_screen_x;
int old_screen_y;

/* Externally-visible functions */

int terminal_open(const uint8_t* filename);

int terminal_read(int32_t fd, void* buf, int32_t nbytes);

int terminal_write(int32_t fd, const void* buf, int32_t nbytes);

int terminal_close(int32_t fd);

void terminal_backspace();

void terminal_clear();

#endif /* _TERMINAL_H */
