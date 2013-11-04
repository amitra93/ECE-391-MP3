/* terminal.c - Terminal driver (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "i8259.h"

int
terminal_open(const uint8_t* filename){
	int i;
	for (i = 0; i < BUFFER_SIZE; i++){
		input[i] = '\0';
	}
	buffer_pointer = 0;
	clear();
	set_cursor_pos(0, 0);
	return 0;
}

int
terminal_read(int32_t fd, void* buf, int32_t nbytes){
	if (buf == NULL){
		return -1;
	}
	int i = 0;
	char* string = (char*) input;
	char* output = (char*) buf;
	while (i < buffer_pointer || string[i] != '\n' || i < nbytes){
		output[i] = string[i];
		i++;
	}
	return 0;
}


int
terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	//fd is 1 for keyboard input, 0 for program input
	if (buf == NULL){
		return -1;
	}
	int i;
	char* string = (char*) buf;
	for (i = 0; i < nbytes; i++){
		if (fd){
			if (string[i] == '\n' || string[i] == '\r'){
				buffer_pointer = 0;
			}
			if (buffer_pointer >= BUFFER_SIZE) {
				return -1;
			}
			input[buffer_pointer] = string[i];
			if (!(string[i] == '\n' || string[i] == '\r')){
				buffer_pointer++;
			}
		}
		get_cursor_pos(&old_screen_x, &old_screen_y);
		printf("%c", string[i]);
	}
	return 0;
}

int
terminal_close(int32_t fd){
	return 0;
}

void terminal_backspace(){
	if (buffer_pointer > 0) buffer_pointer--;
	clear_line(old_screen_y);
	set_cursor_pos(0, old_screen_y);
	terminal_write(0, input, buffer_pointer);
	//if (old_screen_x > 0) old_screen_x--;
}
