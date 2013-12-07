/* keyboard.c - RTC handler (work in progress)
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "sys_calls.h"
#include "sched.h"


/* Scancodes for lowercase characters, 'EOI' char whenever appropriate */
//char lowercase[256] = { '\0','\0','1','2','3','4','5','6','7','8','9','0','-','=','\b','\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n','\0','a','s', 'd','f','g','h','j','k','l',';','\'','`','\0','\\','z','x','c','v', 'b','n','m',',','.','/','\0','*','\0',' ','\0','\0','\0',' ','\0','\0', '\0','\0','\0','\0','\0','\0','\0','7','8','9','-','4','5','6','+','1', '2','3','0','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };

//char uppercase[256] = { '\0','\0','!','@','#','$','%','^','&','*','(',')','_','+','\b','\t', 'Q','W','E','R','T','Y','U','I','O','P','{','}','\n','\0','A','S', 'D','F','G','H','J','K','L',':','"','~','\0','|','Z','X','C','V', 'B','N','M','<','>','?','\0','*','\0',' ','\0','\0','\0',' ','\0','\0', '\0','\0','\0','\0','\0','\0','\0','7','8','9','-','4','5','6','+','1', '2','3','0','.','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0', '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0' };




//__________________KEYBOARD CHARACTER STUFF___________________


 key dummy 			= { '\0', '\0', '\0', 0, 0 }; //this should never be pressed
 key escape 		= { '\0', '\0', '\0', 0, 0 };
 key number_1 		= { '1' , '1' , '!' , 0, 1 };
 key number_2 		= { '2' , '2' , '@' , 0, 1 };
 key number_3 		= { '3' , '3' , '#' , 0, 1 };
 key number_4 		= { '4' , '4' , '$' , 0, 1 };
 key number_5 		= { '5' , '5' , '%' , 0, 1 };
 key number_6 		= { '6' , '6' , '^' , 0, 1 };
 key number_7 		= { '7' , '7' , '&' , 0, 1 };
 key number_8 		= { '8' , '8' , '*' , 0, 1 };
 key number_9 		= { '9' , '9' , '(' , 0, 1 };
 key number_0 		= { '0' , '0' , ')' , 0, 1 };
 key dash 			= { '-' , '-' , '_' , 0, 1 };
 key equals 			= { '=' , '=' , '+' , 0, 1 };
 key backspace 		= { '\0', '\0', '\0', 0, 0 };
 key tab 				= { '\t', '\t', '\t', 0, 1 };
 key q 				= { 'q' , 'Q' , 'Q' , 0, 1 };
 key w 				= { 'w' , 'W' , 'W' , 0, 1 };
 key e 				= { 'e' , 'E' , 'E' , 0, 1 };
 key r 				= { 'r' , 'R' , 'R' , 0, 1 };
 key t 				= { 't' , 'T' , 'T' , 0, 1 };
 key y 				= { 'y' , 'Y' , 'Y' , 0, 1 };
 key u 				= { 'u' , 'U' , 'U' , 0, 1 };
 key i 				= { 'i' , 'I' , 'I' , 0, 1 };
 key o 				= { 'o' , 'O' , 'O' , 0, 1 };
 key p 				= { 'p' , 'P' , 'P' , 0, 1 };
 key left_bracket 	= { '[' , '[' , '{' , 0, 1 };
 key right_bracket 	= { ']' , ']' , '}' , 0, 1 };
 key enter 			= { '\n', '\n', '\n', 0, 1 };
 key left_control 	= { '\0', '\0', '\0', 0, 0 };
 key a 				= { 'a' , 'A' , 'A' , 0, 1 };
 key s 				= { 's' , 'S' , 'S' , 0, 1 };
 key d 				= { 'd' , 'D' , 'D' , 0, 1 };
 key f 				= { 'f' , 'F' , 'F' , 0, 1 };
 key g 				= { 'g' , 'G' , 'G' , 0, 1 };
 key h 				= { 'h' , 'H' , 'H' , 0, 1 };
 key j 				= { 'j' , 'J' , 'J' , 0, 1 };
 key k 				= { 'k' , 'K' , 'K' , 0, 1 };
 key l 				= { 'l' , 'L' , 'L' , 0, 1 };
 key semicolon 		= { ';' , ';' , ':' , 0, 1 };
key single_quote 	= { '\'', '\'', '\"', 0, 1 };
key back_tick 		= { '`' , '`' , '~' , 0, 1 };
key left_shift 		= { '\0', '\0', '\0', 0, 0 };
key backslash 		= { '\\', '\\', '|' , 0, 1 };
key z 				= { 'z' , 'X' , 'Z' , 0, 1 };
key x 				= { 'x' , 'X' , 'X' , 0, 1 };
key c 				= { 'c' , 'C' , 'C' , 0, 1 };
key v 				= { 'v' , 'V' , 'V' , 0, 1 };
key b 				= { 'b' , 'B' , 'B' , 0, 1 };
key n 				= { 'n' , 'N' , 'N' , 0, 1 };
key m 				= { 'm' , 'M' , 'M' , 0, 1 };
key comma 			= { ',' , ',' , '<' , 0, 1 };
key period 			= { '.' , '.' , '>' , 0, 1 };
key slash 			= { '/' , '/' , '?' , 0, 1 };
key right_shift 	= { '\0', '\0', '\0', 0, 0 };
key left_alt 		= { '\0', '\0', '\0', 0, 0 };
key space 			= { ' ' , ' ' , ' ' , 0, 1 };
key caps_lock 		= { '\0', '\0', '\0', 0, 0 };
key f1 				= { '\0', '\0', '\0', 0, 0 };
key f2 				= { '\0', '\0', '\0', 0, 0 };
key f3 				= { '\0', '\0', '\0', 0, 0 };
key f4 				= { '\0', '\0', '\0', 0, 0 };
key f5 				= { '\0', '\0', '\0', 0, 0 };
key f6 				= { '\0', '\0', '\0', 0, 0 };
key f7 				= { '\0', '\0', '\0', 0, 0 };
key f8 				= { '\0', '\0', '\0', 0, 0 };
key f9 				= { '\0', '\0', '\0', 0, 0 };
key f10 			= { '\0', '\0', '\0', 0, 0 };
key f11 			= { '\0', '\0', '\0', 0, 0 };
key f12 			= { '\0', '\0', '\0', 0, 0 };
key number_lock 	= { '\0', '\0', '\0', 0, 0 };
key scroll_lock 	= { '\0', '\0', '\0', 0, 0 };
key keypad_1 		= { '1' , '1' , '1' , 0, 1 };
key keypad_2 		= { '2' , '2' , '2' , 0, 1 };
key keypad_3 		= { '3' , '3' , '3' , 0, 1 };
key keypad_4 		= { '4' , '4' , '4' , 0, 1 };
key keypad_5 		= { '5' , '5' , '5' , 0, 1 };
key keypad_6 		= { '6' , '6' , '6' , 0, 1 };
key keypad_7 		= { '7' , '7' , '7' , 0, 1 };
key keypad_8 		= { '8' , '8' , '8' , 0, 1 };
key keypad_9 		= { '9' , '9' , '9' , 0, 1 };
key keypad_0 		= { '0' , '0' , '0' , 0, 1 };
key keypad_dash 	= { '-' , '-' , '-' , 0, 1 };
key keypad_plus 	= { '+' , '+' , '+' , 0, 1 };
key keypad_period 	= { '.' , '.' , '.' , 0, 1 };
key keypad_star 	= { '*' , '*' , '*' , 0, 1 };
key keypad_enter 	= { '\n', '\n', '\n', 0, 1 };
key right_control 	= { '\0', '\0', '\0', 0, 0 };
key keypad_slash 	= { '/' , '/' , '/' , 0, 1 };
key right_alt 		= { '\0', '\0', '\0', 0, 0 };
key home 			= { '\0', '\0', '\0', 0, 0 };
key cursor_up 		= { '\0', '\0', '\0', 0, 0 };
key page_up 		= { '\0', '\0', '\0', 0, 0 };
key cursor_left 	= { '\0', '\0', '\0', 0, 0 };
key cursor_right 	= { '\0', '\0', '\0', 0, 0 };
key end 			= { '\0', '\0', '\0', 0, 0 };
key cursor_down 	= { '\0', '\0', '\0', 0, 0 };
key page_down 		= { '\0', '\0', '\0', 0, 0 };
key insert 			= { '\0', '\0', '\0', 0, 0 };
key delete 			= { '\0', '\0', '\0', 0, 0 };
key left_gui 		= { '\0', '\0', '\0', 0, 0 };
key right_gui 		= { '\0', '\0', '\0', 0, 0 };
key apps 			= { '\0', '\0', '\0', 0, 0 };
key pause 			= { '\0', '\0', '\0', 0, 0 };



key_orientation dummy_pressed 			= { &dummy 			, 1 }; //this should never be pressed
key_orientation escape_pressed 			= { &escape  		, 1 };
key_orientation number_1_pressed 		= { &number_1  		, 1 };
key_orientation number_2_pressed		= { &number_2  		, 1 };
key_orientation number_3_pressed		= { &number_3  		, 1 };
key_orientation number_4_pressed 		= { &number_4  		, 1 };
key_orientation number_5_pressed 		= { &number_5  		, 1 };
key_orientation number_6_pressed 		= { &number_6  		, 1 };
key_orientation number_7_pressed 		= { &number_7  		, 1 };
key_orientation number_8_pressed 		= { &number_8  		, 1 };
key_orientation number_9_pressed 		= { &number_9  		, 1 };
key_orientation number_0_pressed 		= { &number_0 	 	, 1 };
key_orientation dash_pressed 			= { &dash 	  		, 1 };
key_orientation equals_pressed 			= { &equals  		, 1 };
key_orientation backspace_pressed 		= { &backspace 		, 1 };
key_orientation tab_pressed 			= { &tab 	  		, 1 };
key_orientation q_pressed 				= { &q  	 		, 1 };
key_orientation w_pressed 				= { &w  	 		, 1 };
key_orientation e_pressed 				= { &e  	 		, 1 };
key_orientation r_pressed 				= { &r  	 		, 1 };
key_orientation t_pressed 				= { &t  	 		, 1 };
key_orientation y_pressed 				= { &y  	 		, 1 };
key_orientation u_pressed 				= { &u  	 		, 1 };
key_orientation i_pressed 				= { &i  	 		, 1 };
key_orientation o_pressed 				= { &o  	 		, 1 };
key_orientation p_pressed 				= { &p  	 		, 1 };
key_orientation left_bracket_pressed 	= { &left_bracket 	, 1 };
key_orientation right_bracket_pressed 	= { &right_bracket  , 1 };
key_orientation enter_pressed 			= { &enter  	 	, 1 };
key_orientation left_control_pressed 	= { &left_control  	, 1 };
key_orientation a_pressed 				= { &a 				, 1 };
key_orientation s_pressed 				= { &s 				, 1 };
key_orientation d_pressed 				= { &d 				, 1 };
key_orientation f_pressed 				= { &f 				, 1 };
key_orientation g_pressed 				= { &g 				, 1 };
key_orientation h_pressed 				= { &h 				, 1 };
key_orientation j_pressed 				= { &j 				, 1 };
key_orientation k_pressed 				= { &k 				, 1 };
key_orientation l_pressed 				= { &l 				, 1 };
key_orientation semicolon_pressed 		= { &semicolon 		, 1 };
key_orientation single_quote_pressed 	= { &single_quote	, 1 };
key_orientation back_tick_pressed 		= { &back_tick 		, 1 };
key_orientation left_shift_pressed 		= { &left_shift 	, 1 };
key_orientation backslash_pressed 		= { &backslash 		, 1 };
key_orientation z_pressed 				= { &z 				, 1 };
key_orientation x_pressed 				= { &x 				, 1 };
key_orientation c_pressed 				= { &c 				, 1 };
key_orientation v_pressed 				= { &v 				, 1 };
key_orientation b_pressed 				= { &b 				, 1 };
key_orientation n_pressed 				= { &n 				, 1 };
key_orientation m_pressed 				= { &m 				, 1 };
key_orientation comma_pressed 			= { &comma 			, 1 };
key_orientation period_pressed 			= { &period 		, 1 };
key_orientation slash_pressed 			= { &slash 			, 1 };
key_orientation right_shift_pressed 	= { &right_shift 	, 1 };
key_orientation left_alt_pressed  		= { &left_alt 		, 1 };
key_orientation space_pressed 			= { &space 			, 1 };
key_orientation caps_lock_pressed 		= { &caps_lock 		, 1 };
key_orientation f1_pressed 				= { &f1 			, 1 };
key_orientation f2_pressed 				= { &f2				, 1 };
key_orientation f3_pressed 				= { &f3				, 1 };
key_orientation f4_pressed 				= { &f4				, 1 };
key_orientation f5_pressed 				= { &f5				, 1 };
key_orientation f6_pressed 				= { &f6 			, 1 };
key_orientation f7_pressed 				= { &f7				, 1 };
key_orientation f8_pressed 				= { &f8				, 1 };
key_orientation f9_pressed 				= { &f9				, 1 };
key_orientation f10_pressed 			= { &f10 			, 1 };
key_orientation f11_pressed 			= { &f11			, 1 };
key_orientation f12_pressed 			= { &f12			, 1 };
key_orientation number_lock_pressed 	= { &number_lock 	, 1 };
key_orientation scroll_lock_pressed 	= { &scroll_lock 	, 1 };
key_orientation keypad_1_pressed 		= { &keypad_1 		, 1 };
key_orientation keypad_2_pressed 		= { &keypad_2 		, 1 };
key_orientation keypad_3_pressed 		= { &keypad_3 		, 1 };
key_orientation keypad_4_pressed 		= { &keypad_4 		, 1 };
key_orientation keypad_5_pressed 		= { &keypad_5 		, 1 };
key_orientation keypad_6_pressed 		= { &keypad_6 		, 1 };
key_orientation keypad_7_pressed 		= { &keypad_7 		, 1 };
key_orientation keypad_8_pressed 		= { &keypad_8 		, 1 };
key_orientation keypad_9_pressed 		= { &keypad_9 		, 1 };
key_orientation keypad_0_pressed 		= { &keypad_0 		, 1 };
key_orientation keypad_dash_pressed 	= { &keypad_dash 	, 1 };
key_orientation keypad_plus_pressed 	= { &keypad_plus 	, 1 };
key_orientation keypad_period_pressed 	= { &keypad_period 	, 1 };
key_orientation keypad_star_pressed 	= { &keypad_star 	, 1 };
key_orientation keypad_enter_pressed 	= { &keypad_enter 	, 1 };
key_orientation right_control_pressed 	= { &right_control 	, 1 };
key_orientation keypad_slash_pressed 	= { &keypad_slash 	, 1 };
key_orientation right_alt_pressed 		= { &right_alt  	, 1 };
key_orientation home_pressed 			= { &home 		 	, 1 };
key_orientation cursor_up_pressed 		= { &cursor_up 	 	, 1 };
key_orientation page_up_pressed 		= { &page_up 	 	, 1 };
key_orientation cursor_left_pressed 	= { &cursor_left 	, 1 };
key_orientation cursor_right_pressed 	= { &cursor_right 	, 1 };
key_orientation end_pressed 			= { &end 		 	, 1 };
key_orientation cursor_down_pressed 	= { &cursor_down  	, 1 };
key_orientation page_down_pressed 		= { &page_down 	 	, 1 };
key_orientation insert_pressed 			= { &insert 	 	, 1 };
key_orientation delete_pressed 			= { &delete 	 	, 1 };
key_orientation left_gui_pressed 		= { &left_gui 	 	, 1 };
key_orientation right_gui_pressed 		= { &right_gui 	 	, 1 };
key_orientation apps_pressed 			= { &apps 		 	, 1 };
key_orientation pause_pressed 			= { &pause 			, 1 };



key_orientation dummy_released 			= { &dummy 			, 0 }; //this should never be released
key_orientation escape_released 		= { &escape  		, 0 };
key_orientation number_1_released 		= { &number_1  		, 0 };
key_orientation number_2_released		= { &number_2  		, 0 };
key_orientation number_3_released		= { &number_3  		, 0 };
key_orientation number_4_released 		= { &number_4  		, 0 };
key_orientation number_5_released 		= { &number_5  		, 0 };
key_orientation number_6_released 		= { &number_6  		, 0 };
key_orientation number_7_released 		= { &number_7  		, 0 };
key_orientation number_8_released 		= { &number_8  		, 0 };
key_orientation number_9_released 		= { &number_9  		, 0 };
key_orientation number_0_released 		= { &number_0 	 	, 0 };
key_orientation dash_released 			= { &dash 	  		, 0 };
key_orientation equals_released 		= { &equals  		, 0 };
key_orientation backspace_released 		= { &backspace 		, 0 };
key_orientation tab_released 			= { &tab 	  		, 0 };
key_orientation q_released 				= { &q  	 		, 0 };
key_orientation w_released 				= { &w  	 		, 0 };
key_orientation e_released 				= { &e  	 		, 0 };
key_orientation r_released 				= { &r  	 		, 0 };
key_orientation t_released 				= { &t  	 		, 0 };
key_orientation y_released 				= { &y  	 		, 0 };
key_orientation u_released 				= { &u  	 		, 0 };
key_orientation i_released 				= { &i  	 		, 0 };
key_orientation o_released 				= { &o  	 		, 0 };
key_orientation p_released 				= { &p  	 		, 0 };
key_orientation left_bracket_released 	= { &left_bracket 	, 0 };
key_orientation right_bracket_released 	= { &right_bracket  , 0 };
key_orientation enter_released 			= { &enter  	 	, 0 };
key_orientation left_control_released 	= { &left_control  	, 0 };
key_orientation a_released 				= { &a 				, 0 };
key_orientation s_released 				= { &s 				, 0 };
key_orientation d_released 				= { &d 				, 0 };
key_orientation f_released 				= { &f 				, 0 };
key_orientation g_released 				= { &g 				, 0 };
key_orientation h_released 				= { &h 				, 0 };
key_orientation j_released 				= { &j 				, 0 };
key_orientation k_released 				= { &k 				, 0 };
key_orientation l_released 				= { &l 				, 0 };
key_orientation semicolon_released 		= { &semicolon 		, 0 };
key_orientation single_quote_released 	= { &single_quote	, 0 };
key_orientation back_tick_released 		= { &back_tick 		, 0 };
key_orientation left_shift_released 	= { &left_shift 	, 0 };
key_orientation backslash_released 		= { &backslash 		, 0 };
key_orientation z_released 				= { &z 				, 0 };
key_orientation x_released 				= { &x 				, 0 };
key_orientation c_released 				= { &c 				, 0 };
key_orientation v_released 				= { &v 				, 0 };
key_orientation b_released 				= { &b 				, 0 };
key_orientation n_released 				= { &n 				, 0 };
key_orientation m_released 				= { &m 				, 0 };
key_orientation comma_released 			= { &comma 			, 0 };
key_orientation period_released 		= { &period 		, 0 };
key_orientation slash_released 			= { &slash 			, 0 };
key_orientation right_shift_released 	= { &right_shift 	, 0 };
key_orientation left_alt_released  		= { &left_alt 		, 0 };
key_orientation space_released 			= { &space 			, 0 };
key_orientation caps_lock_released 		= { &caps_lock 		, 0 };
key_orientation f1_released 			= { &f1 			, 0 };
key_orientation f2_released 			= { &f2				, 0 };
key_orientation f3_released 			= { &f3				, 0 };
key_orientation f4_released 			= { &f4				, 0 };
key_orientation f5_released 			= { &f5				, 0 };
key_orientation f6_released 			= { &f6 			, 0 };
key_orientation f7_released 			= { &f7				, 0 };
key_orientation f8_released 			= { &f8				, 0 };
key_orientation f9_released 			= { &f9				, 0 };
key_orientation f10_released 			= { &f10 			, 0 };
key_orientation f11_released 			= { &f11			, 0 };
key_orientation f12_released 			= { &f12			, 0 };
key_orientation number_lock_released 	= { &number_lock 	, 0 };
key_orientation scroll_lock_released 	= { &scroll_lock 	, 0 };
key_orientation keypad_1_released 		= { &keypad_1 		, 0 };
key_orientation keypad_2_released 		= { &keypad_2 		, 0 };
key_orientation keypad_3_released 		= { &keypad_3 		, 0 };
key_orientation keypad_4_released 		= { &keypad_4 		, 0 };
key_orientation keypad_5_released 		= { &keypad_5 		, 0 };
key_orientation keypad_6_released 		= { &keypad_6 		, 0 };
key_orientation keypad_7_released 		= { &keypad_7 		, 0 };
key_orientation keypad_8_released 		= { &keypad_8 		, 0 };
key_orientation keypad_9_released 		= { &keypad_9 		, 0 };
key_orientation keypad_0_released 		= { &keypad_0 		, 0 };
key_orientation keypad_dash_released 	= { &keypad_dash 	, 0 };
key_orientation keypad_plus_released 	= { &keypad_plus 	, 0 };
key_orientation keypad_period_released 	= { &keypad_period 	, 0 };
key_orientation keypad_star_released 	= { &keypad_star 	, 0 };
key_orientation keypad_enter_released 	= { &keypad_enter 	, 0 };
key_orientation right_control_released 	= { &right_control 	, 0 };
key_orientation keypad_slash_released 	= { &keypad_slash 	, 0 };
key_orientation right_alt_released 		= { &right_alt  	, 0 };
key_orientation home_released 			= { &home 		 	, 0 };
key_orientation cursor_up_released 		= { &cursor_up 	 	, 0 };
key_orientation page_up_released 		= { &page_up 	 	, 0 };
key_orientation cursor_left_released 	= { &cursor_left 	, 0 };
key_orientation cursor_right_released 	= { &cursor_right 	, 0 };
key_orientation end_released 			= { &end 		 	, 0 };
key_orientation cursor_down_released 	= { &cursor_down  	, 0 };
key_orientation page_down_released 		= { &page_down 	 	, 0 };
key_orientation insert_released 		= { &insert 	 	, 0 };
key_orientation delete_released 		= { &delete 	 	, 0 };
key_orientation left_gui_released 		= { &left_gui 	 	, 0 };
key_orientation right_gui_released 		= { &right_gui 	 	, 0 };
key_orientation apps_released 			= { &apps 		 	, 0 };
key_orientation pause_released 			= { &pause 			, 0 };


key_orientation* keymap[0xFF];


void init_keys(){
	int i;
	for (i = 0; i < 0xFF; i++){
		keymap[i] = &dummy_pressed;
	}

	keymap[0x01] = &escape_pressed;
	keymap[0x02] = &number_1_pressed;
	keymap[0x03] = &number_2_pressed;
	keymap[0x04] = &number_3_pressed;
	keymap[0x05] = &number_4_pressed;
	keymap[0x06] = &number_5_pressed;
	keymap[0x07] = &number_6_pressed;
	keymap[0x08] = &number_7_pressed;
	keymap[0x09] = &number_8_pressed;
	keymap[0x0A] = &number_9_pressed;
	keymap[0x0B] = &number_0_pressed;
	keymap[0x0C] = &dash_pressed;
	keymap[0x0D] = &equals_pressed;
	keymap[0x0E] = &backspace_pressed;
	keymap[0x0F] = &tab_pressed;
	keymap[0x10] = &q_pressed;
	keymap[0x11] = &w_pressed;
	keymap[0x12] = &e_pressed;
	keymap[0x13] = &r_pressed;
	keymap[0x14] = &t_pressed;
	keymap[0x15] = &y_pressed;
	keymap[0x16] = &u_pressed;
	keymap[0x17] = &i_pressed;
	keymap[0x18] = &o_pressed;
	keymap[0x19] = &p_pressed;
	keymap[0x1A] = &left_bracket_pressed;
	keymap[0x1B] = &right_bracket_pressed;
	keymap[0x1C] = &enter_pressed;
	keymap[0x1D] = &left_control_pressed;
	keymap[0x1E] = &a_pressed;
	keymap[0x1F] = &s_pressed;
	keymap[0x20] = &d_pressed;
	keymap[0x21] = &f_pressed;
	keymap[0x22] = &g_pressed;
	keymap[0x23] = &h_pressed;
	keymap[0x24] = &j_pressed;
	keymap[0x25] = &k_pressed;
	keymap[0x26] = &l_pressed;
	keymap[0x27] = &semicolon_pressed;
	keymap[0x28] = &single_quote_pressed;
	keymap[0x29] = &back_tick_pressed;
	keymap[0x2A] = &left_shift_pressed;
	keymap[0x2B] = &backslash_pressed;
	keymap[0x2C] = &z_pressed;
	keymap[0x2D] = &x_pressed;
	keymap[0x2E] = &c_pressed;
	keymap[0x2F] = &v_pressed;
	keymap[0x30] = &b_pressed;
	keymap[0x31] = &n_pressed;
	keymap[0x32] = &m_pressed;
	keymap[0x33] = &comma_pressed;
	keymap[0x34] = &period_pressed;
	keymap[0x35] = &slash_pressed;
	keymap[0x36] = &right_shift_pressed;
	keymap[0x37] = &keypad_star_pressed;
	keymap[0x38] = &left_alt_pressed;
	keymap[0x39] = &space_pressed;
	keymap[0x3A] = &caps_lock_pressed;
	keymap[0x3B] = &f1_pressed;
	keymap[0x3C] = &f2_pressed;
	keymap[0x3D] = &f3_pressed;
	keymap[0x3E] = &f4_pressed;
	keymap[0x3F] = &f5_pressed;
	keymap[0x40] = &f6_pressed;
	keymap[0x41] = &f7_pressed;
	keymap[0x42] = &f8_pressed;
	keymap[0x43] = &f9_pressed;
	keymap[0x44] = &f10_pressed;
	keymap[0x45] = &number_lock_pressed;
	keymap[0x46] = &scroll_lock_pressed;
	keymap[0x47] = &keypad_7_pressed;
	keymap[0x48] = &keypad_8_pressed;
	keymap[0x49] = &keypad_9_pressed;
	keymap[0x4A] = &keypad_dash_pressed;
	keymap[0x4B] = &keypad_4_pressed;
	keymap[0x4C] = &keypad_5_pressed;
	keymap[0x4D] = &keypad_6_pressed;
	keymap[0x4E] = &keypad_plus_pressed;
	keymap[0x4F] = &keypad_1_pressed;
	keymap[0x50] = &keypad_2_pressed;
	keymap[0x51] = &keypad_3_pressed;
	keymap[0x52] = &keypad_0_pressed;
	keymap[0x53] = &keypad_period_pressed;

	keymap[0x57] = &f11_pressed;
	keymap[0x58] = &f12_pressed;

	keymap[0x81] = &escape_released;
	keymap[0x82] = &number_1_released;
	keymap[0x83] = &number_2_released;
	keymap[0x84] = &number_3_released;
	keymap[0x85] = &number_4_released;
	keymap[0x86] = &number_5_released;
	keymap[0x87] = &number_6_released;
	keymap[0x88] = &number_7_released;
	keymap[0x89] = &number_8_released;
	keymap[0x8A] = &number_9_released;
	keymap[0x8B] = &number_0_released;
	keymap[0x8C] = &dash_released;
	keymap[0x8D] = &equals_released;
	keymap[0x8E] = &backspace_released;
	keymap[0x8F] = &tab_released;
	keymap[0x90] = &q_released;
	keymap[0x91] = &w_released;
	keymap[0x92] = &e_released;
	keymap[0x93] = &r_released;
	keymap[0x94] = &t_released;
	keymap[0x95] = &y_released;
	keymap[0x96] = &u_released;
	keymap[0x97] = &i_released;
	keymap[0x98] = &o_released;
	keymap[0x99] = &p_released;
	keymap[0x9A] = &left_bracket_released;
	keymap[0x9B] = &right_bracket_released;
	keymap[0x9C] = &enter_released;
	keymap[0x9D] = &left_control_released;
	keymap[0x9E] = &a_released;
	keymap[0x9F] = &s_released;
	keymap[0xA0] = &d_released;
	keymap[0xA1] = &f_released;
	keymap[0xA2] = &g_released;
	keymap[0xA3] = &h_released;
	keymap[0xA4] = &j_released;
	keymap[0xA5] = &k_released;
	keymap[0xA6] = &l_released;
	keymap[0xA7] = &semicolon_released;
	keymap[0xA8] = &single_quote_released;
	keymap[0xA9] = &back_tick_released;
	keymap[0xAA] = &left_shift_released;
	keymap[0xAB] = &backslash_released;
	keymap[0xAC] = &z_released;
	keymap[0xAD] = &x_released;
	keymap[0xAE] = &c_released;
	keymap[0xAF] = &v_released;
	keymap[0xB0] = &b_released;
	keymap[0xB1] = &n_released;
	keymap[0xB2] = &m_released;
	keymap[0xB3] = &comma_released;
	keymap[0xB4] = &period_released;
	keymap[0xB5] = &slash_released;
	keymap[0xB6] = &right_shift_released;
	keymap[0xB7] = &keypad_star_released;
	keymap[0xB8] = &left_alt_released;
	keymap[0xB9] = &space_released;
	keymap[0xBA] = &caps_lock_pressed;
	keymap[0xBB] = &f1_released;
	keymap[0xBC] = &f2_released;
	keymap[0xBD] = &f3_released;
	keymap[0xBE] = &f4_released;
	keymap[0xBF] = &f5_released;
	keymap[0xC0] = &f6_released;
	keymap[0xC1] = &f7_released;
	keymap[0xC2] = &f8_released;
	keymap[0xC3] = &f9_released;
	keymap[0xC4] = &f10_released;
	keymap[0xC5] = &number_lock_released;
	keymap[0xC6] = &scroll_lock_released;
	keymap[0xC7] = &keypad_7_released;
	keymap[0xC8] = &keypad_8_released;
	keymap[0xC9] = &keypad_9_released;
	keymap[0xCA] = &keypad_dash_released;
	keymap[0xCB] = &keypad_4_released;
	keymap[0xCC] = &keypad_5_released;
	keymap[0xCD] = &keypad_6_released;
	keymap[0xCE] = &keypad_plus_released;
	keymap[0xCF] = &keypad_1_released;
	keymap[0xD0] = &keypad_2_released;
	keymap[0xD1] = &keypad_3_released;
	keymap[0xD2] = &keypad_0_released;
	keymap[0xD3] = &keypad_period_released;

	keymap[0xD7] = &f11_released;
	keymap[0xD8] = &f12_released;
}



unsigned char is_shift_pressed(){
	return (is_pressed(&right_shift) || is_pressed(&left_shift));
}

unsigned char is_enter_pressed(){
	return (is_pressed(&enter) || is_pressed(&keypad_enter));
}

unsigned char is_caps_lock_pressed(){
	return is_pressed(&caps_lock);
}

unsigned char is_control_pressed(){
	return (is_pressed(&right_control) || is_pressed(&left_control));
}

unsigned char is_alt_pressed(){
	return (is_pressed(&right_alt) || is_pressed(&left_alt));
}

unsigned char is_pressed(key* keyboard_key){
	return keyboard_key->pressed;
}

unsigned char print_key(key* keyboard_key){
	if (keyboard_key == NULL || !(keyboard_key->pressed) || !(keyboard_key->to_display) || shortcut_received){
		return -1;
	}
	unsigned char char_to_print = '\0';
	if (is_shift_pressed()){
		char_to_print = keyboard_key->shift_char;
	}
	else if (is_caps_lock_pressed()){
		char_to_print = keyboard_key->uppercase_char;
	}
	else {
		char_to_print = keyboard_key->lowercase_char;
	}
	terminal_add_to_buffer(char_to_print);
	return 0;
}


//_________END KEYBOARD CHARACTER STUFF____________________







/* Initialize the keyboard */
void
keyboard_init(void)
{
	// keyboard is automatically enabled, all we do is enable IRQ1
	
	// set constants to use
	//array_to_use = &lowercase[0];
	//is_uppercase = 0;
	//is_control_pressed = 0;
	enable_irq(1);
	init_keys();
	shortcut_received = 0;
	//keyboard_last_printable_key = '\0';
}

void process_shortcuts(void){
	if (is_control_pressed() && is_pressed(&l)){
		terminal_clear();
		set_cursor_pos(0, 0);
		shortcut_received = 1;
		return;
	}
	if (is_control_pressed() && is_pressed(&c)){
		//printf("\nWill halt later\n");
		shortcut_received = 0;
		send_eoi(1);
		halt(1);
		return;
	}
	if (is_pressed(&backspace)){
		terminal_backspace();
		backspace.pressed = 0;
		shortcut_received = 1;
		return;
	}
}

void
process_keypress(void)
{
	// read scancode from data port
	unsigned char scan_code = inb(DATA_PORT);
	keymap[scan_code]->keyboard_key->pressed = keymap[scan_code]->orientation;
	process_shortcuts();
	print_key(keymap[scan_code]->keyboard_key);
	shortcut_received = 0;
	
}

unsigned char keyboard_wait_for_new_line(int max_chars){
	/*
	int i = 0;
	while (i < max_chars){
		while (!keyboard_interrupt_received);
		i++;
		keyboard_interrupt_received = 0;
		if (keyboard_last_printable_key == '\n'){
			return i;
		}
		
		
	}
	*/
	terminal* current_terminal = get_current_terminal();
	int old_input_pointer = current_terminal->input.input_pointer;
	while (old_input_pointer > current_terminal->input.input_pointer - max_chars || current_terminal->ptid != schedular.cur_ptree){
		if (is_enter_pressed() && current_terminal->input.input_pointer < BUFFER_SIZE-1){
			enter.pressed = 0;
			keypad_enter.pressed = 0;
			return get_last_terminal_line()->input_pointer - old_input_pointer;
		}
		
	}
	
	return max_chars;
}




