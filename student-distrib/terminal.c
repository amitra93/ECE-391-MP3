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
	while (i < buffer_pointer){
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
		printf("%c", string[i]);
		if (fd){
			input[buffer_pointer] = string[i];
			buffer_pointer++;
			//if (character == '\n'){
			//	buffer_pointer = 0;
			//}
		}
	}
	return 0;
}

int
terminal_close(int32_t fd){
	return 0;
}
