/*

Faults
	The saved value of eip is the address of the instruction that caused the fault, 
	and hence that instruction can be resumed when the exception handler terminates. 
	As we shall see in Section 7.4 in Chapter 7, resuming the same instruction is 
	necessary whenever the handler is able to correct the anomalous condition that 
	caused the exception.

Traps
	The saved value of eip is the address of the instruction that should be 
	executed after the one that caused the trap. A trap is triggered only when there 
	is no need to reexecute the instruction that terminated. The main use of traps 
	is for debugging purposes: the role of the interrupt signal in this case is to 
	notify the debugger that a specific instruction has been executed (for instance, 
	a breakpoint has been reached within a program). Once the user has examined the 
	data provided by the debugger, she may ask that execution of the debugged program
	resume starting from the next instruction.

Aborts
	A serious error occurred; the control unit is in trouble, and it may be unable to 
	store a meaningful value in the eip register. Aborts are caused by hardware failures 
	or by invalid values in system tables. The interrupt signal sent by the control unit
	is an emergency signal used to switch control to the corresponding abort exception 
	handler. This handler has no choice but to force the affected process to terminate.
*/

#include "lib.h"

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

void print_error(char * description, uint32_t error_code, uint32_t instr_ptr, uint32_t pid)
{
	int32_t str_len, x, y;
	
	str_len = strlen(description);
	clear();
	x = (NUM_COLS / 2) - 16;
	y = (NUM_ROWS / 2) - 2;
	
	set_cursor_pos(x, y);
	printf("Error       : %s", description);
	
	x = (NUM_COLS / 2);
	y ++;
	
	set_cursor_pos((NUM_COLS / 2) - 16, y++);
	printf("Error Code  : 0x%#x", error_code);
	
	set_cursor_pos((NUM_COLS / 2) - 16, y++);
	printf("Instruction : 0x%#x", instr_ptr);
	
	set_cursor_pos((NUM_COLS / 2) - 16, y++);
	printf("Process ID  : 0x%#x", pid);
}

void do_idt_unknown_intr(void){
	print_error("Unknown Interrupt", 0, 0, 0);
} 

/* Interrupt 0 : Divide Error (fault) */
void do_idt_intel_de(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Divide by Zero", error, instr_ptr, pid);
} 

/* Interrupt 1 : Debug (trap or fault)
examine the contents of DR6 and other debug registers */
void do_idt_intel_db(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Debug", error, instr_ptr, pid);
} 

/* Interrupt 2 : Not Used */
void do_idt_intel_nmi(void){ } 

/* Interrupt 3 : Breakpoint (trap) */
void do_idt_intel_bp(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Breakpoint", error, instr_ptr, pid);
} 

/* Interrupt 4 : Overflow (trap) */
void do_idt_intel_of(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Overflow", error, instr_ptr, pid);
} 

/* Interrupt 5 : Bounds Check (fault) */
void do_idt_intel_br(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Bounds Check Fault", error, instr_ptr, pid);
} 

/* Interrupt 6 : Invalid Opcode (fault) */
void do_idt_intel_ud(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Invalid Opcode", error, instr_ptr, pid);
} 

/* Interrupt 7 : Device not Available (fault) */
void do_idt_intel_nm(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Device Not Available", error, instr_ptr, pid);
} 

/* Interrupt 8 : Double Fault (abort) */
void do_idt_intel_df(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Double Fault", error, instr_ptr, pid);
}  

/* Interrupt 9 : Coprocessor Segment Overrun (abort) */
void do_idt_intel_cso(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Coprocessor Segment Overrun", error, instr_ptr, pid);
}  

/* Interrupt 10 : Invalid TSS (fault) */
void do_idt_intel_ts(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Invalid TSS", error, instr_ptr, pid);
} 

/* Interrupt 11 : Segment not Present (fault) */
void do_idt_intel_np(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Segment Not Present", error, instr_ptr, pid);
} 

/* Interrupt 12 : Stack Segment (fault) */
void do_idt_intel_ss(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Stack Segment Fault", error, instr_ptr, pid);
} 

/* Interrupt 13 : General Protection (fault) */
void do_idt_intel_gp(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("General Protection Fault", error, instr_ptr, pid);
} 

/* Interrupt 14 : Page Fault (fault) */
void do_idt_intel_pf(void){
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Page Fault", error, instr_ptr, pid);
	while(1);
} 

/* Interrupt 16 : Floating Point Error (fault) */
void do_idt_intel_mf(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Floating Point Fault", error, instr_ptr, pid);
} 

/* Interrupt 17 : Alignment Check (fault) */
void do_idt_intel_ac(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Alignment Check Fault", error, instr_ptr, pid);
} 

/* Interrupt 18 : Machine-Check Exception (abort) */
void do_idt_intel_mc(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Machine-Check Exception", error, instr_ptr, pid);
} 

/* Interrupt 19 : Floating-Point Exception (fault) */
void do_idt_intel_xf(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Floating-Point Exception", error, instr_ptr, pid);
} 

