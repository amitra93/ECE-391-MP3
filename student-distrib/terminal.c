/* terminal.c - Terminal driver (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "sched.h"

char* video_mem;

int
terminal_open(const uint8_t* filename){	
	int i, j, k;
	current_terminal_index = 0;
	video_mem = (char *)VIDEO;
	for (i = 0; i < MAX_SUPPORTED_TERMINALS; i++){
		terminal_list[i].screen_x = 0;
		terminal_list[i].screen_y = 0;
		terminal_list[i].history_index = 0;
		terminal_list[i].chars_printed = 0;
		terminal_list[i].starting_offset = 0;
		terminal_list[i].input.input_pointer = 0;
		terminal_list[i].ptid = (unsigned int)i;
		for (j = 0; j < BUFFER_SIZE; j++){
			terminal_list[i].input.line[j] = '\0';
		}
		for (j = 0; j < MAX_SUPPORTED_HISTORY; j++){
			terminal_list[i].input_history[j].input_pointer = 0;
			for (k = 0; k < BUFFER_SIZE; k++){
				terminal_list[i].input_history[j].line[k] = '\0';
			}
		}

		//TODO change this....
		//terminal_list[i].video_buffer = (char*)(VIRTUAL_VID_MEM + VIDEO00);
		for (j = 0; j < NUM_ROWS * NUM_COLS * 2; j++){
			terminal_list[i].video_buffer[j] = 0x0;
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
	int temp;
	terminal* current_terminal = get_current_terminal();
	get_cursor_pos(&current_terminal->starting_offset, &temp);
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
	
	//Change this so that it will add to the history when it isn't the current terminal
	//if (current_terminal->ptid == schedular.cur_ptree)
	//{
		set_cursor_pos( current_terminal->screen_x, current_terminal->screen_y);
		if (buf == NULL){
			return -1;
		}
		if (nbytes <= 0){
			return 0;
		}
		char* string = (char*) buf;
		for (i = 0; i < nbytes; i++){
			if (current_terminal->chars_printed > NUM_COLS){
				char newline = '\n';
				printf("%c", newline);
				current_terminal->chars_printed = 0;
			}
			else if (string[i] == '\n'){
				current_terminal->chars_printed = 0;
			}
			else if (string[i] == '\0'){
				continue;
			}
			printf("%c", string[i]);
			current_terminal->chars_printed++;
		}
		get_cursor_pos(&current_terminal->screen_x, &current_terminal->screen_y);
	//}
	return 0;
}

int
terminal_close(int32_t fd){
	return 0;
}

void terminal_backspace(){
	char str = ' ';
	terminal* current_terminal = get_current_terminal();
	if (current_terminal->input.input_pointer <= 0){
		return;
	}
	if (current_terminal->input.input_pointer != (NUM_COLS - current_terminal->starting_offset)){
		current_terminal->screen_x--;
	}
	else {
		if (current_terminal->screen_y > 0){
			current_terminal->screen_y--;
		}
		current_terminal->screen_x = NUM_COLS - 1;
	}
	current_terminal->chars_printed--;
	current_terminal->input.input_pointer--;
	set_cursor_pos(current_terminal->screen_x, current_terminal->screen_y);
	printf("%c", str);
	/*
	if (current_terminal->screen_x <= 0){
		current_terminal->screen_x = NUM_COLS - 1;
		current_terminal->screen_y--;
	}
	*/
	/*
	if (current_terminal->input.input_pointer <= NUM_COLS - current_terminal->starting_offset){
		if (current_terminal->screen_x == 0){
			current_terminal->screen_x = NUM_COLS;
		}
		current_terminal->screen_x -= current_terminal->input.input_pointer;
		terminal_write(1,&current_terminal->input.line, current_terminal->input.input_pointer-1);
		char str = ' ';
		printf("%c",str);
		current_terminal->input.input_pointer--;
	}
	else {
		current_terminal->screen_x -= (current_terminal->input.input_pointer + NUM_COLS - current_terminal->starting_offset);
		terminal_write(1,&current_terminal->input.line + NUM_COLS - current_terminal->starting_offset, current_terminal->input.input_pointer - 1 - NUM_COLS + current_terminal->starting_offset);
		char str = ' ';
		printf("%c",str);
		current_terminal->input.input_pointer--;
	}
	*/
}

void terminal_clear(){
	clear();
	terminal* current_terminal = get_current_terminal();
	current_terminal->screen_x = current_terminal->screen_y = 0;
	set_cursor_pos(0, 0);
	
}


void terminal_add_to_buffer(unsigned char char_to_print){
	terminal* current_terminal = get_current_terminal();
	//if (current_terminal->ptid == schedular.cur_ptree){
		if (current_terminal->input.input_pointer >= BUFFER_SIZE-1){
			return;
		}
		current_terminal->input.line[current_terminal->input.input_pointer++] = char_to_print;
		if (char_to_print == '\n'){
			terminal_copy_to_history();
			current_terminal->input.input_pointer = 0;
		}

		terminal_write(1, &char_to_print, 1);
		if (current_terminal->screen_x == 0 && current_terminal->input.input_pointer >= NUM_COLS - current_terminal->starting_offset){
			current_terminal->screen_y++;
			if (current_terminal->screen_y >= NUM_ROWS -1){
				scroll_up();
			}
			current_terminal->chars_printed = 0;
		}
	//}
}

terminal* get_current_terminal(){
	return &terminal_list[current_terminal_index];
}

terminal_line* get_last_terminal_line(){
	//int temp = 0;
	terminal* current_terminal = get_current_terminal();
	//if (current_terminal->history_index > 0){
	//	temp = current_terminal->history_index - 1;
	//}
	return &current_terminal->previous_input;
}

void terminal_copy_to_history(){
	int i = 0;
	int8_t* shell_name = "shell";
	int8_t* cur_task_name = (int8_t*) get_cur_task()->pName;
	int not_copy_to_history = strncmp(cur_task_name, shell_name, 5);
	terminal* current_terminal = get_current_terminal();
	if (!not_copy_to_history){
		current_terminal->input_history[current_terminal->history_index].input_pointer = current_terminal->input.input_pointer;
	}
	current_terminal->previous_input.input_pointer = current_terminal->input.input_pointer;
	while (i < current_terminal->input.input_pointer){
		current_terminal->previous_input.line[i] = current_terminal->input.line[i];
		if (!not_copy_to_history){
			current_terminal->input_history[current_terminal->history_index].line[i] = current_terminal->input.line[i];
		}
		i++;
	}
	if (!not_copy_to_history){
		current_terminal->history_index++;
	}
}

void set_current_terminal(int terminal_index){
	if (current_terminal_index == terminal_index || terminal_index < 0 || terminal_index > 2){
		return;
	}
	//copy active video memory to buffer
	memcpy(&get_current_terminal()->video_buffer, video_mem, NUM_COLS*NUM_ROWS*2);

	current_terminal_index = terminal_index;
	//do the reverse
	memcpy(video_mem, &get_current_terminal()->video_buffer, NUM_COLS*NUM_ROWS*2);
	//get_current_terminal()->video_memory = (char*)(VIRTUAL_VID_MEM + VIDEO);

	//TODO memcopy buff into memory
	return;
}

