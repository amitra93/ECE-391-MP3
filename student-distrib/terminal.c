/* terminal.c - Terminal driver (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

int
terminal_open(const uint8_t* filename){
	int i;
	for (i = 0; i < BUFFER_SIZE; i++){
		input[i] = '\0';
	}
	on_new_line = 0;
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
	char* output = (char*) buf;
	output[i] = '\0';
	while (i < nbytes){
		output[i] = keyboard_get_last_printable_key();
		if (output[i] == '\n')
			break;
		i++;
	}
	return i;
}


int
terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	//fd is 1 for keyboard input, 0 for program input
	if (buf == NULL){
		return -1;
	}
	int i;
	char* string = (char*) buf;
	cli();
	for (i = 0; i < nbytes; i++){
		if (fd){
			if (buffer_pointer >= BUFFER_SIZE) {
				return -1;
			}
			if (string[i] == '\n' || string[i] == '\r'){
				buffer_pointer = 0;
				on_new_line = 0;
			}
			if (buffer_pointer >= NUM_COLS && !on_new_line){
				char newline = '\n';
				printf("%c", newline);
				on_new_line = 1;
			}
			input[buffer_pointer] = string[i];
			if (!(string[i] == '\n' || string[i] == '\r')){
				buffer_pointer++;
			}
		}
		get_cursor_pos(&old_screen_x, &old_screen_y);
		printf("%c", string[i]);
	}
	sti();
	return 0;
}

int
terminal_close(int32_t fd){
	return 0;
}

void terminal_backspace(){
	if (buffer_pointer <= SHELL_OFFSET){
		return;
	}
	buffer_pointer--;
	clear_line(old_screen_y);
	if (buffer_pointer > 0 && on_new_line && old_screen_x == 0 && old_screen_y > 0){
		on_new_line = 0;
		old_screen_y--;
	}
	set_cursor_pos(0, old_screen_y);
	if (buffer_pointer < NUM_COLS) terminal_write
		(0, input, buffer_pointer);
	else terminal_write(0, input+NUM_COLS, buffer_pointer-NUM_COLS);
	//if (old_screen_x > 0) old_screen_x--;
}

void terminal_clear(){
	clear();
	buffer_pointer = 0;
	set_cursor_pos(0, 0);
}

void test_terminal(){
	char buf[250] = { [0 ... 249] = '1' };
	char str = '\n';
	char* huge_nbytes = "\nhuge nbytes\n";
	char* small_nbytes = "\nsmall nbytes\n";
	terminal_read(0, &buf, 5000);
	printf(huge_nbytes);
	printf(buf);
	printf("%c", str);
	int i;
	for (i = 0; i < 250; i++){
		buf[i] = '1';
	}
	printf(small_nbytes);
	terminal_read(0, &buf, 20);
	printf(buf);
	printf("%c", str);
}

