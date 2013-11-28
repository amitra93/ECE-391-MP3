#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "rtc.h"
#include "pit.h"

/* Constants */
#define PIT			0
#define KEYBOARD	1
#define RTC			8

void do_idt_keyboard(void){
	process_keypress();
	// need to send EOI else no more interrupts will be generated
	send_eoi(KEYBOARD);
}

void do_idt_rtc(void){
	//test_interrupts();
	rtc_process_interrupt();
	
	// need to send EOI else no more interrupts will be generated
	send_eoi(RTC);
}

void do_idt_pit(void){
	pit_process_interrupt();
	
	send_eoi(PIT);
}
