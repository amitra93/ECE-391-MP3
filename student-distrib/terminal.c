/* terminal.c - Terminal driver (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "sched.h"

#define ATTRIB 0x7

//external video_mem pointer from lib.c
char* video_mem;

/*
 * int terminal_open(const uint8_t* filename)
 * DESCRIPTION: 
 *
 * INPUTS: filename
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
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
		terminal_list[i].starting_offset = SHELL_OFFSET;
		terminal_list[i].input.input_pointer = 0;
		terminal_list[i].ptid = (unsigned int)i;
		terminal_list[i].state = TERMINAL_IDLE;
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
			terminal_list[i].video_buffer[j<<1] = ' ';
			terminal_list[i].video_buffer[(j<<1) + 1] = ATTRIB;
		}
	}
	clear();
	set_cursor_pos(0, 0);
	return 0;
}

/*
 * int terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * DESCRIPTION: 
 *
 * INPUTS: buf, nbytes
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
int
terminal_read(int32_t fd, void* buf, int32_t nbytes){
	if (buf == NULL){
		return -1;
	}
	int temp;
	terminal* current_terminal = get_displaying_terminal();
	get_cursor_pos(&current_terminal->starting_offset, &temp);
	if (get_executing_terminal()->input.input_pointer > 0){
		terminal_write(1, get_executing_terminal()->input.line, get_executing_terminal()->input.input_pointer);
	}
	if (nbytes <= 0){
		return 0;
	}
	if (nbytes > BUFFER_SIZE){
		nbytes = BUFFER_SIZE;
	}
	get_executing_terminal()->state = TERMINAL_READ;
	char* output = (char*) buf;
	int i = keyboard_wait_for_new_line(nbytes);
	int j = 0;
	while (j < i-1){
		output[j] = get_last_terminal_line()->line[j];
		j++;
	}
	get_executing_terminal()->state = TERMINAL_IDLE;
	return i-1;
	
}

/*
 * int terminal_write(int32_t fd, const void* buf, int32_t nbytes)
 * DESCRIPTION: 
 *
 * INPUTS: fd, buf, nbytes
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
int
terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	int i;
	cli();
	terminal* current_exe_terminal = get_executing_terminal();
	terminal* current_disp_terminal = get_displaying_terminal();

	if (buf == NULL){
		sti();
		return -1;
	}
	if (nbytes <= 0){
		sti();
		return 0;
	}
	char* string = (char*) buf;
	for (i = 0; i < nbytes; i++){
		if (current_exe_terminal->chars_printed > NUM_COLS){
			char newline = '\n';
			if (current_exe_terminal == current_disp_terminal)
			{
				set_cursor_pos( current_exe_terminal->screen_x, current_exe_terminal->screen_y);
				printf("%c", newline);
				get_cursor_pos(&current_exe_terminal->screen_x, &current_exe_terminal->screen_y);
			}
			else
				vprintf(current_exe_terminal, "%c", newline);
			current_exe_terminal->chars_printed = 0;
		}
		else if (string[i] == '\n'){
			current_exe_terminal->chars_printed = 0;
		}
		else if (string[i] == '\0'){
			continue;
		}
		if (current_exe_terminal == current_disp_terminal)
		{
			set_cursor_pos( current_exe_terminal->screen_x, current_exe_terminal->screen_y);
			printf("%c", string[i]);
			get_cursor_pos(&current_exe_terminal->screen_x,&current_exe_terminal->screen_y);
		}
		else
			vprintf(current_exe_terminal, "%c", string[i]);
		current_exe_terminal->chars_printed++;
	}
	set_blinking_cursor_pos( current_disp_terminal->screen_x, current_disp_terminal->screen_y);
	sti();
	return 0;
}


/*
 * int terminal_write_keypress(unsigned char * buf, int32_t nbytes)
 * DESCRIPTION: 
 *
 * INPUTS: buf, nbytes
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
int
terminal_write_keypress(unsigned char * buf, int32_t nbytes){
	int i;
	
	terminal* current_disp_terminal = get_displaying_terminal();
	
	if (buf == NULL){
		return -1;
	}
	if (nbytes <= 0){
		return 0;
	}
	char* string = (char*) buf;
	for (i = 0; i < nbytes; i++){
		if (current_disp_terminal->chars_printed > NUM_COLS){
			char newline = '\n';
			set_cursor_pos( current_disp_terminal->screen_x, current_disp_terminal->screen_y);
			printf("%c", newline);
			get_cursor_pos(&current_disp_terminal->screen_x, &current_disp_terminal->screen_y);
			current_disp_terminal->chars_printed = 0;
		}
		else if (string[i] == '\n'){
			current_disp_terminal->chars_printed = 0;
		}
		else if (string[i] == '\0'){
			continue;
		}
		set_cursor_pos( current_disp_terminal->screen_x, current_disp_terminal->screen_y);
		printf("%c", string[i]);
		get_cursor_pos(&current_disp_terminal->screen_x,&current_disp_terminal->screen_y);

		set_blinking_cursor_pos(current_disp_terminal->screen_x, current_disp_terminal->screen_y);
		current_disp_terminal->chars_printed++;
	}

	return 0;
}

/*
 * int terminal_close(int32_t fd)
 * DESCRIPTION: 
 *
 * INPUTS: fd
 * OUTPUTS: returns 0
 * SIDE EFFECTS: 
 */
int
terminal_close(int32_t fd){
	return 0;
}

/*
 * void terminal_backspace()
 * DESCRIPTION: 
 *
 * INPUTS: none
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
void terminal_backspace(){
	char str = ' ';
	terminal* current_terminal = get_displaying_terminal();
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
	if (current_terminal->screen_x == 0){
		set_blinking_cursor_pos( current_terminal->screen_x, current_terminal->screen_y-1);
	}
	else {
		set_blinking_cursor_pos( current_terminal->screen_x, current_terminal->screen_y);
	}
	printf("%c", str);

}

/*
 * void terminal_clear()
 * DESCRIPTION: 
 *
 * INPUTS: none
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
void terminal_clear(){
	clear();
	terminal* current_terminal = get_displaying_terminal();
	current_terminal->screen_x = current_terminal->screen_y = 0;
	set_cursor_pos(0, 0);
	set_blinking_cursor_pos(0, 0);
}

/*
 * void terminal_add_to_buffer(unsigned char char_to_print)
 * DESCRIPTION: 
 *
 * INPUTS: none
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
void terminal_add_to_buffer(unsigned char char_to_print){
	terminal* current_terminal = get_displaying_terminal();
	//if (current_terminal->ptid == scheduler.cur_ptree){
		if (current_terminal->input.input_pointer >= BUFFER_SIZE-1){
			return;
		}
		current_terminal->input.line[current_terminal->input.input_pointer++] = char_to_print;
		if (char_to_print == '\n'){
			current_terminal->state = TERMINAL_DONE_READ;
			terminal_copy_to_history();
			current_terminal->input.input_pointer = 0;
		}

		terminal_write_keypress(&char_to_print, 1);
		if (current_terminal->screen_x == 0 && current_terminal->input.input_pointer >= NUM_COLS - current_terminal->starting_offset){
			current_terminal->screen_y++;
			if (current_terminal->screen_y >= NUM_ROWS -1){
				scroll_up((unsigned char *)video_mem);
			}
			current_terminal->chars_printed = 0;
		}
	//}
}

/*
 * terminal* get_displaying_terminal()
 * DESCRIPTION: 
 *
 * INPUTS: none
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
terminal* get_displaying_terminal(){
	return &terminal_list[current_terminal_index];
}

/*
 * terminal* get_executing_terminal()
 * DESCRIPTION: 
 *
 * INPUTS: none
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
terminal * get_executing_terminal(){
	return &terminal_list[scheduler.cur_ptree];
}

/*
 * terminal_line* get_last_terminal_line()
 * DESCRIPTION: 
 *
 * INPUTS: none
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
terminal_line* get_last_terminal_line(){
	//int temp = 0;
	terminal* current_terminal = get_displaying_terminal();
	//if (current_terminal->history_index > 0){
	//	temp = current_terminal->history_index - 1;
	//}
	return &current_terminal->previous_input;
}

/*
 * void terminal_copy_to_history()
 * DESCRIPTION: 
 *
 * INPUTS: none
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
void terminal_copy_to_history(){
	int i = 0;
	int8_t* shell_name = "shell";
	int8_t* cur_task_name = (int8_t*) get_cur_task()->pName;
	int not_copy_to_history = strncmp(cur_task_name, shell_name, 5);
	terminal* current_terminal = get_displaying_terminal();
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
		current_terminal->history_index = (current_terminal->history_index+1)%MAX_SUPPORTED_HISTORY;
	}
}

/*
 * void set_current_terminal(int terminal_index)
 * DESCRIPTION: 
 *
 * INPUTS: none
 * OUTPUTS: 
 * SIDE EFFECTS: 
 */
void set_current_terminal(int terminal_index){
	if (current_terminal_index == terminal_index || terminal_index < 0 || terminal_index > 2){
		return;
	}

	
	//copy active video memory to buffer
	memcpy(&get_displaying_terminal()->video_buffer, video_mem, NUM_COLS*NUM_ROWS*2);

	current_terminal_index = terminal_index;
	//do the reverse
	memcpy(video_mem, &get_displaying_terminal()->video_buffer, NUM_COLS*NUM_ROWS*2);
	set_blinking_cursor_pos(terminal_list[terminal_index].screen_x, terminal_list[terminal_index].screen_y);
	//get_displaying_terminal()->video_memory = (char*)(VIRTUAL_VID_MEM + VIDEO);
	
	//TODO memcopy buff into memory
	return;
}

