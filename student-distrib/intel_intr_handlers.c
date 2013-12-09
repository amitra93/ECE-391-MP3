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
#include "sys_calls.h"
#include "terminal.h"
#include "sched.h"
/*
*  GET_ARGS(error, instr_ptr, pid)
*  Description: Gets the arguments from registers
*  Inputs: Unsigned Int * to variables to hold error code, instruction pointer, and process id
*  Outputs: error gets error code, instr_ptr gets instruction pointer, pid gets process id
*  Side Effects: Overwrites inputs
*/
#define GET_ARGS(error, instr_ptr, pid) 					\
	do { 									 				\
		asm volatile("										\
		movl %%eax, %0 \n movl %%ebx, %1 \n movl %%ecx, %2" \
		:"=d"(error), "=S"(instr_ptr), "=D"(pid));			\
	}while(0)						

/*
*  Interrupt Handlers
*  Description: Prints an error code on screen
*  Inputs: EAX = Error Code, EBX = Instruction Pointer, ECX = Process ID
*  Outputs: None
*  Side Effects: Clears screen and prints error code. Individual interrupts have 
*				 their own side effects to handle the exception.
*/

void do_common_intr(void){
	uint8_t key;
	sti();
	terminal_read(0, &key, 1);
	set_cur_task_state(TASK_EXCEPTION);
	halt(0);
}

/* Generate an unknown interrupt exception */
void do_idt_unknown_intr(void){
	print_error("Unknown Interrupt", 0, 0, 0);
	return do_common_intr();
} 

/* Interrupt 0 : Divide Error (fault) */
void do_idt_intel_de(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Divide by Zero", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 1 : Debug (trap or fault)
examine the contents of DR6 and other debug registers */
void do_idt_intel_db(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Debug", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 2 : Not Used, for now */
void do_idt_intel_nmi(void){ } 

/* Interrupt 3 : Breakpoint (trap) */
void do_idt_intel_bp(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Breakpoint", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 4 : Overflow (trap) */
void do_idt_intel_of(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Overflow", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 5 : Bounds Check (fault) */
void do_idt_intel_br(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Bounds Check Fault", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 6 : Invalid Opcode (fault) */
void do_idt_intel_ud(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Invalid Opcode", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 7 : Device not Available (fault) */
void do_idt_intel_nm(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Device Not Available", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 8 : Double Fault (abort) */
void do_idt_intel_df(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Double Fault", error, instr_ptr, pid);
	return do_common_intr();
}  

/* Interrupt 9 : Coprocessor Segment Overrun (abort) */
void do_idt_intel_cso(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Coprocessor Segment Overrun", error, instr_ptr, pid);
	return do_common_intr();
}  

/* Interrupt 10 : Invalid TSS (fault) */
void do_idt_intel_ts(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Invalid TSS", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 11 : Segment not Present (fault) */
void do_idt_intel_np(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Segment Not Present", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 12 : Stack Segment (fault) */
void do_idt_intel_ss(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Stack Segment Fault", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 13 : General Protection (fault) */
void do_idt_intel_gp(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("General Protection Fault", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 14 : Page Fault (fault) */
void do_idt_intel_pf(void){
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Page Fault", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 16 : Floating Point Error (fault) */
void do_idt_intel_mf(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Floating Point Fault", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 17 : Alignment Check (fault) */
void do_idt_intel_ac(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Alignment Check Fault", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 18 : Machine-Check Exception (abort) */
void do_idt_intel_mc(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Machine-Check Exception", error, instr_ptr, pid);
	return do_common_intr();
} 

/* Interrupt 19 : Floating-Point Exception (fault) */
void do_idt_intel_xf(void){ 
	uint32_t error, instr_ptr, pid;
	GET_ARGS(error, instr_ptr, pid);
	print_error("Floating-Point Exception", error, instr_ptr, pid);
	return do_common_intr();
} 
