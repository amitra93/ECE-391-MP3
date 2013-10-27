/* keyboard.c - RTC handler (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "lib.h"


// global variable to select between uppercase and lowercase letters
char* array_to_use;

// determine status of whether SHIFT/CAPS is pressed or not
int is_uppercase;

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


/* Initialize the keyboard */
void
keyboard_init(void)
{
	// keyboard is automatically enabled, all we do is enable IRQ1
	
	// set constants to use
	array_to_use = &lowercase[0];
	is_uppercase = 0;
}

void
process_keypress(void)
{
	// read scancode from data port
	unsigned char char_pressed = inb(DATA_PORT);
	// determine case (SHIFT or CAPS might have been pressed)
	determine_case(char_pressed);
	// convert scancode to ASCII character
	unsigned char char_to_print = array_to_use[char_pressed];
	// if character is a letter or number (not a special char), print it
	if (char_to_print != '\0') printf("%c", char_to_print);
}
