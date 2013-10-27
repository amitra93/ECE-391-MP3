#include "lib.h"
#include "i8259.h"

#define GET_ARGS(error, instr_ptr, pid) 					\
	do { 									 				\
		asm volatile("										\
		movl %%eax, %0 \n movl %%ebx, %1 \n movl %%ecx, %2" \
		:"=d"(error), "=S"(instr_ptr), "=D"(pid));			\
	}while(0)						

#define VIDEO 0xB8000
#define NUM_COLS 80
#define NUM_ROWS 25
#define ATTRIB 0x7



/*___________HARDWARE INTERRUPTS___________*/

void do_idt_keyboard(void){
	printf("Entering KB handler\n");
	send_eoi(1);
}

void do_idt_rtc(void){

}
