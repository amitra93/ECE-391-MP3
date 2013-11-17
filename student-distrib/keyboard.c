/* keyboard.c - RTC handler (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "keyboard_characters.h"




/* Scancodes for lowercase characters, 'EOI' char whenever appropriate */
char lowercase[256] = { '\0','\0','1','2','3','4','5','6','7','8','9','0','-','=','\b','\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n','\0','a','s', 'd','f','g','h','j','k','l',';','\'','`','\0','\\','z','x','c','v', 'b','n','m',',','.','/','\0','*','\0',' ','\0','\0','\0',' ','\0','\0', '\0','\0','\0','\0','\0','\0','\0','7','8','9','-','4','5','6','+','1', '2','3','0','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };

char uppercase[256] = { '\0','\0','!','@','#','$','%','^','&','*','(',')','_','+','\b','\t', 'Q','W','E','R','T','Y','U','I','O','P','{','}','\n','\0','A','S', 'D','F','G','H','J','K','L',':','"','~','\0','|','Z','X','C','V', 'B','N','M','<','>','?','\0','*','\0',' ','\0','\0','\0',' ','\0','\0', '\0','\0','\0','\0','\0','\0','\0','7','8','9','-','4','5','6','+','1', '2','3','0','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };


void determine_case(unsigned char char_pressed){
	// if SHIFT is held or CAPS is pressed, change status of case variable
	if (char_pressed == LSHIFT_PRESS || char_pressed == RSHIFT_PRESS || char_pressed == LSHIFT_RELEASE || char_pressed == RSHIFT_RELEASE || char_pressed == CAPS_PRESS){
		is_uppercase = !is_uppercase;
	}
	
	// select relevant array based on case variable
	if (is_uppercase){
		array_to_use = &uppercase[0];
	}
	else {
		array_to_use = &lowercase[0];
	}
}

void change_keys_pressed(unsigned char char_pressed, unsigned char char_to_print){
	// TODO - implement
}


/* Initialize the keyboard */
void
keyboard_init(void)
{
	// keyboard is automatically enabled, all we do is enable IRQ1
	
	// set constants to use
	array_to_use = &lowercase[0];
	is_uppercase = 0;
	is_control_pressed = 0;
	enable_irq(1);
	keyboard_interrupt_received = 0;
	keyboard_last_printable_key = '\0';
}

void
process_keypress(void)
{
	// read scancode from data port
	unsigned char char_pressed = inb(DATA_PORT);
	// convert scancode to ASCII character
	unsigned char char_to_print = array_to_use[char_pressed];

	// determine case (SHIFT or CAPS might have been pressed)
	determine_case(char_pressed);

	// change keyboard map (maintain history of what's pressed and released)
	change_keys_pressed(char_pressed, char_to_print);


	if (char_pressed == LCNTL_PRESS){
		is_control_pressed = 1;
	}
	if (char_pressed == LCNTL_RELEASE){
		is_control_pressed = 0;
	}
	if (is_control_pressed && char_pressed == L_PRESS){
		terminal_clear();
		set_cursor_pos(0, 0);
		return;
	}
	if (is_control_pressed && char_pressed == LSHIFT_PRESS){
		test_terminal();
		return;
	}
	if (char_pressed == BKSP_PRESS){
		terminal_backspace();
		return;
	}

	keyboard_last_printable_key = char_to_print;
	
	// if character is a letter or number (not a special char), print it
	if (char_to_print != '\0') {
		terminal_write(1, &char_to_print, 1);
		keyboard_interrupt_received = 1;
	}
	//printf("-->%x<--\n", char_pressed);
	
}

unsigned char keyboard_wait_for_new_line(int max_chars){
	int i = 0;
	while (i < max_chars){
		while (!keyboard_interrupt_received);
		i++;
		keyboard_interrupt_received = 0;
		if (keyboard_last_printable_key == '\n'){
			break;
		}
		
		
	}

	return i;
}
