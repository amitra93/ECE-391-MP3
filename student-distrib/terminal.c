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
 * DESCRIPTION: initializes our 3 terminals and sets up relevant video buffers
 *
 * INPUTS: filename
 * OUTPUTS: none
 * SIDE EFFECTS: none
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
			//initialize history and current input buffers to null string
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
	//clear the string and set cursor position to top
	clear();
	set_cursor_pos(0, 0);

	return 0;
}

/*
 * int terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * DESCRIPTION: blocking function that reads from the terminal until nbytes characters or newline has been pressed
 *
 * INPUTS: buf, nbytes
 * OUTPUTS: success or failure
 * SIDE EFFECTS: blocks the current terminal until newline has been pressed
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
		//if user typed stuff while program was executing, print that before waiting
		terminal_write(1, get_executing_terminal()->input.line, get_executing_terminal()->input.input_pointer);
	}
	//check for invalid input
	if (nbytes <= 0){
		return 0;
	}
	if (nbytes > BUFFER_SIZE){
		nbytes = BUFFER_SIZE;
	}
	//set state of terminal to reading
	get_executing_terminal()->state = TERMINAL_READ;
	char* output = (char*) buf;
	int i = keyboard_wait_for_new_line(nbytes);
	int j = 0;
	while (j < i-1){
		//copy input from internal keyboard buffer to buf
		output[j] = get_last_terminal_line()->line[j];
		j++;
	}
	//set state of terminal back after we are done
	get_executing_terminal()->state = TERMINAL_IDLE;

	//return number of bytes copied
	return i-1;
	
}

/*
 * int terminal_write(int32_t fd, const void* buf, int32_t nbytes)
 * DESCRIPTION: writes nbytes characters from buf to terminal
 *
 * INPUTS: fd, buf, nbytes
 * OUTPUTS: changes state of the currently executing terminal
 * SIDE EFFECTS: 
 */
int
terminal_write(int32_t fd, const void* buf, int32_t nbytes){
	int i;

	//need to not interrupt terminal with keyboard input
	cli();

	terminal* current_exe_terminal = get_executing_terminal();
	terminal* current_disp_terminal = get_displaying_terminal();

	//check for invalid input
	if (buf == NULL){
		sti();
		return -1;
	}
	if (nbytes <= 0){
		sti();
		return 0;
	}

	//cast void pointer to string to avoid compiler warnings
	char* string = (char*) buf;
	for (i = 0; i < nbytes; i++){

		//if buf has more than 80 characters we need to print to a new line
		if (current_exe_terminal->chars_printed > NUM_COLS){
			char newline = '\n';
			if (current_exe_terminal == current_disp_terminal)
			{
				//maintain cursor position inside terminal struct
				set_cursor_pos( current_exe_terminal->screen_x, current_exe_terminal->screen_y);
				printf("%c", newline);
				get_cursor_pos(&current_exe_terminal->screen_x, &current_exe_terminal->screen_y);
			}
			else
				vprintf(current_exe_terminal, "%c", newline);
			current_exe_terminal->chars_printed = 0;
		}
		//reset number of chars printed if we encounter newline in buf
		else if (string[i] == '\n'){
			current_exe_terminal->chars_printed = 0;
		}

		//skip null characters in buf
		else if (string[i] == '\0'){
			continue;
		}

		//print to currnt terminal or buffer depending on which terminal is executing
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

	//set blinking cursor to position after last drawn character
	set_blinking_cursor_pos( current_disp_terminal->screen_x, current_disp_terminal->screen_y);
	
	//set IF flag again
	sti();

	return 0;
}


/*
 * int terminal_write_keypress(unsigned char * buf, int32_t nbytes)
 * DESCRIPTION: outputs to terminal, but this function is only called from the keyboard interrupt handler
 *
 * INPUTS: buf, nbytes
 * OUTPUTS: 
 * SIDE EFFECTS: outputs keypresses to current terminal only
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

		//if buf has more than 80 chars print a newline
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

		//skip null chars
		else if (string[i] == '\0'){
			continue;
		}

		//maintain cursor positions inside struct
		set_cursor_pos( current_disp_terminal->screen_x, current_disp_terminal->screen_y);
		printf("%c", string[i]);
		get_cursor_pos(&current_disp_terminal->screen_x,&current_disp_terminal->screen_y);

		//set blinking cursor to position after last drawn char
		set_blinking_cursor_pos(current_disp_terminal->screen_x, current_disp_terminal->screen_y);
		
		//maintain struct variables
		current_disp_terminal->chars_printed++;
	}

	return 0;
}

/*
 * int terminal_close(int32_t fd)
 * DESCRIPTION: Returns success because we can't really close terminal
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
 * DESCRIPTION: Deletes the last pressed character from current terminal while maintaining state/position variables
 *
 * INPUTS: none
 * OUTPUTS: returns 0
 * SIDE EFFECTS: 
 */
void terminal_backspace(){
	char str = ' ';
	terminal* current_terminal = get_displaying_terminal();
	//don't delete if nothing has been pressed
	if (current_terminal->input.input_pointer <= 0){
		return;
	}

	//don't delete too many characters
	if (current_terminal->input.input_pointer != (NUM_COLS - current_terminal->starting_offset)){
		current_terminal->screen_x--;
	}
	else {
		//go up to last line if conditions are met
		if (current_terminal->screen_y > 0){
			current_terminal->screen_y--;
		}
		current_terminal->screen_x = NUM_COLS - 1;
	}
	//maintain terminal state/position variables
	current_terminal->chars_printed--;
	current_terminal->input.input_pointer--;
	set_cursor_pos(current_terminal->screen_x, current_terminal->screen_y);

	//set blinking cursor position based on current screen position variables
	if (current_terminal->screen_x == 0){
		set_blinking_cursor_pos( current_terminal->screen_x, current_terminal->screen_y-1);
	}
	else {
		set_blinking_cursor_pos( current_terminal->screen_x, current_terminal->screen_y);
	}
	//finally print a space over the last pressed character
	printf("%c", str);

}

/*
 * void terminal_clear()
 * DESCRIPTION: cleans the screen
 *
 * INPUTS: none
 * OUTPUTS: returns 0
 * SIDE EFFECTS: sets text and cursor position to top
 */
void terminal_clear(){
	//clear the screen
	clear();

	terminal* current_terminal = get_displaying_terminal();

	//reset terminal position variables
	current_terminal->screen_x = current_terminal->screen_y = 0;

	//set text and blinking cursor position to the top
	set_cursor_pos(0, 0);
	set_blinking_cursor_pos(0, 0);
}

/*
 * void terminal_add_to_buffer(unsigned char char_to_print)
 * DESCRIPTION: Adds last pressed keyboard key to internal buffer
 *
 * INPUTS: none
 * OUTPUTS: return 0 or -1 based on if we could add to buffer or not
 * SIDE EFFECTS: prints last pressed key to console
 */
void terminal_add_to_buffer(unsigned char char_to_print){

	//only add to currently displaying terminal
	terminal* current_terminal = get_displaying_terminal();

	//don't add to buffer if buffer is full
	if (current_terminal->input.input_pointer >= BUFFER_SIZE-1){
		return;
	}

	//add to buffer
	current_terminal->input.line[current_terminal->input.input_pointer++] = char_to_print;

	//if last pressed char is newline, we are done accepting input
	if (char_to_print == '\n'){
		current_terminal->state = TERMINAL_DONE_READ;
		terminal_copy_to_history();
		current_terminal->input.input_pointer = 0;
	}

	//print last pressed character to console
	terminal_write_keypress(&char_to_print, 1);
	if (current_terminal->screen_x == 0 && current_terminal->input.input_pointer >= NUM_COLS - current_terminal->starting_offset){
		current_terminal->screen_y++;
		if (current_terminal->screen_y >= NUM_ROWS -1){
			//scroll up screen if we are on the last row
			scroll_up((unsigned char *)video_mem);
		}
		//if we are on new line, reset chars_printed
		current_terminal->chars_printed = 0;
	}
}

/*
 * terminal* get_displaying_terminal()
 * DESCRIPTION: returns address of currently displayed terminal
 *
 * INPUTS: none
 * OUTPUTS: address of displayed terminal
 * SIDE EFFECTS: none
 */
terminal* get_displaying_terminal(){
	return &terminal_list[current_terminal_index];
}

/*
 * terminal* get_executing_terminal()
 * DESCRIPTION: returns address of currently executing terminal
 *
 * INPUTS: none
 * OUTPUTS: address of executing terminal
 * SIDE EFFECTS: none
 */
terminal * get_executing_terminal(){
	return &terminal_list[scheduler.cur_ptree];
}

/*
 * terminal_line* get_last_terminal_line()
 * DESCRIPTION: Get previous input line (for keyboard_wait_for_new_line)
 *
 * INPUTS: none
 * OUTPUTS: address of last input buffer
 * SIDE EFFECTS: none
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
 * DESCRIPTION: Copy last pressed terminal line to input history
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: none 
 */
void terminal_copy_to_history(){
	int i = 0;

	//only copy inputs from shell into history array
	int8_t* shell_name = "shell";
	int8_t* cur_task_name = (int8_t*) get_cur_task()->pName;
	int not_copy_to_history = strncmp(cur_task_name, shell_name, 5);

	//else only copy to previous_input variable (needed for hello)
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
 * DESCRIPTION: sets currently displayed terminal to terminal indicated by terminal_index
 *
 * INPUTS: none
 * OUTPUTS: none
 * SIDE EFFECTS: changes contents of video memory 
 */
void set_current_terminal(int terminal_index){
	//check for invalid input
	if (current_terminal_index == terminal_index || terminal_index < 0 || terminal_index > 2){
		return;
	}

	//copy active video memory to buffer
	memcpy(&get_displaying_terminal()->video_buffer, video_mem, NUM_COLS*NUM_ROWS*2);

	current_terminal_index = terminal_index;

	//copy new terminal's video buffer to main video memory
	memcpy(video_mem, &get_displaying_terminal()->video_buffer, NUM_COLS*NUM_ROWS*2);

	//set blinking cursor position
	set_blinking_cursor_pos(terminal_list[terminal_index].screen_x, terminal_list[terminal_index].screen_y);

	return;
}

