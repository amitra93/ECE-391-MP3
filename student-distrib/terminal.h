/* terminal.h - Terminal driver (work in progress)
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"

/* Constants */
#define MAX_SUPPORTED_TERMINALS			3
#define NUM_COLS						80
#define NUM_ROWS						25
#define BUFFER_SIZE						128
#define MAX_SUPPORTED_HISTORY			16

typedef struct terminal_line {
	int input_pointer;
	unsigned char line[BUFFER_SIZE];
} terminal_line;

typedef struct terminal {
	int screen_x;
	int screen_y;
	int history_index;
	volatile int in_use;
	terminal_line input;
	char video_memory[NUM_ROWS * NUM_COLS];
	terminal_line input_history[MAX_SUPPORTED_HISTORY];
} terminal;



terminal terminal_list[MAX_SUPPORTED_TERMINALS];

int current_terminal_index;

int terminal_open(const uint8_t* filename);

int terminal_read(int32_t fd, void* buf, int32_t nbytes);

int terminal_write(int32_t fd, const void* buf, int32_t nbytes);

int terminal_close(int32_t fd);

void terminal_backspace();

void terminal_clear();

void terminal_add_to_buffer(unsigned char char_to_print);

terminal* get_current_terminal();

terminal_line* get_last_terminal_line();

void terminal_copy_to_history();

void set_current_terminal(int terminal_index);

#endif /* _TERMINAL_H */
