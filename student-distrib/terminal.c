/* terminal.c - Terminal driver (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

int
terminal_open(const uint8_t* filename){	
	int i, j, k;
	current_terminal_index = 0;
	for (i = 0; i < MAX_SUPPORTED_TERMINALS; i++){
		terminal_list[i].in_use = 0;
		terminal_list[i].screen_x = 0;
		terminal_list[i].screen_y = 0;
		terminal_list[i].history_index = 0;
		terminal_list[i].input.input_pointer = 0;
		for (j = 0; j < BUFFER_SIZE; j++){
			terminal_list[i].input.line[j] = '\0';
		}
		for (j = 0; j < MAX_SUPPORTED_HISTORY; j++){
			terminal_list[i].input_history[j].input_pointer = 0;
			for (k = 0; k < BUFFER_SIZE; k++){
				terminal_list[i].input_history[j].line[k] = '\0';
			}
		}
		for (j = 0; j < NUM_ROWS * NUM_COLS; j++){
			terminal_list[i].video_memory[j] = ' ';
		}
	}
	clear();
	set_cursor_pos(0, 0);
	return 0;
}

int
terminal_read(int32_t fd, void* buf, int32_t nbytes){
	if (buf == NULL){
		return -1;
	}
	if (nbytes <= 0){
		return 0;
	}
	if (nbytes > BUFFER_SIZE){
		nbytes = BUFFER_SIZE;
	}
	char* output = (char*) buf;
	int i = keyboard_wait_for_new_line(nbytes);
	int j = 0;
	while (j < i-1){
		output[j] = get_last_terminal_line()->line[j];
		j++;
	}
	return i-1;
	
}


int
terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	int i;
	terminal* current_terminal = get_current_terminal();
	if (buf == NULL || fd != 1 || current_terminal->in_use){
		return -1;
	}
	if (nbytes <= 0){
		return 0;
	}
	char* string = (char*) buf;
	current_terminal->in_use = 1;
	for (i = 0; i < nbytes; i++){
		if (i % nbytes == 0 && i > 0){
			char newline = '\n';
			printf("%c", newline);
		}
		printf("%c", string[i]);
	}
	current_terminal->in_use = 0;
	return 0;
}

int
terminal_close(int32_t fd){
	return 0;
}

void terminal_backspace(){

}

void terminal_clear(){
	clear();
	terminal* current_terminal = get_current_terminal();
	current_terminal->screen_x = current_terminal->screen_y = 0;
	set_cursor_pos(0, 0);
	
}


void terminal_add_to_buffer(unsigned char char_to_print){
	terminal* current_terminal = get_current_terminal();
	if (current_terminal->input.input_pointer >= BUFFER_SIZE){
		return;
	}
	current_terminal->input.line[current_terminal->input.input_pointer++] = char_to_print;
	if (char_to_print == '\n'){
		terminal_copy_to_history();
		current_terminal->input.input_pointer = 0;
	}
	if (!(current_terminal->in_use)){
		terminal_write(1, &char_to_print, 1);
	}
}

terminal* get_current_terminal(){
	return &terminal_list[current_terminal_index];
}

terminal_line* get_last_terminal_line(){
	int temp = 0;
	terminal* current_terminal = get_current_terminal();
	if (current_terminal->history_index > 0){
		temp = current_terminal->history_index - 1;
	}
	return &current_terminal->input_history[temp];
}

void terminal_copy_to_history(){
	int i = 0;
	terminal* current_terminal = get_current_terminal();
	current_terminal->input_history[current_terminal->history_index].input_pointer = current_terminal->input.input_pointer;
	while (i < current_terminal->input.input_pointer){
		current_terminal->input_history[current_terminal->history_index].line[i] = current_terminal->input.line[i];
		i++;
	}
	current_terminal->history_index++;
}

void set_current_terminal(int terminal_index){
	current_terminal_index = terminal_index;
	//other stuff here later
}

