#include "pit.h"
#include "lib.h"
#include "i8259.h"
#include "sched.h"
#include "task.h"

#define PIT_IRQ_NUM 0

#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_1 0x41
#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND 0x43
#define PIT_FREQUENCY 1193182
#define PIT_MAX_FREQ PIT_FREQUENCY
#define PIT_MIN_FREQ 19

#define INIT_FREQUENCY 300
#define PIT_DIVISOR (PIT_FREQUENCY/INIT_FREQUENCY)

#define SELECT_CHANNEL_0			0x00
#define SELECT_CHANNEL_1			0x40
#define SELECT_CHANNEL_2			0x80
#define SELECT_CHANNEL_READ_BACK 	0xC0

#define ACCESS_MODE_LATCH_COUNT		0x00
#define ACCESS_MODE_LOBYTE			0x10
#define ACCESS_MODE_HIBYTE			0x20
#define ACCESS_MODE_LOHIBYTE		0x30

#define OPERATING_MODE_0			0x0 //interrupt on terminal count
#define OPERATING_MODE_1			0x2	//hardware re-triggerable one-shot
#define OPERATING_MODE_2			0x4	//rate generator
#define OPERATING_MODE_3			0x6	//square wave generator
#define OPERATING_MODE_4			0x8	//software triggered strobe
#define OPERATING_MODE_5			0xA	//hardware triggered strobe

#define BINARY_MODE					0x0 //16-bit Binary
#define BCD_MODE					0x1 //Four-digit BCD

#define get_cs(cs)\
		do {					\
		asm volatile("			\
			movw 36(%%ebp), %0;	\
			":"=r"(cs));		\
		}while(0)
	
#define setup_task_stack(task)\
	do {												\
		asm volatile("									\
			movw %0, 48(%%ebp)			;				\
			movl %1, 44(%%ebp) 			;				\
			movl %2, 40(%%ebp)			;				\
			movw %3, 36(%%ebp)			;				\
			movl %4, 32(%%ebp)			;				\
			movw $0x2b, %%ax 			;				\
			movw %%ax, %%ds				;				\
			"::"g"((task)->tss.ss), 					\
			"g"((task)->tss.esp), 						\
			"g"((task)->tss.eflags),					\
			"g"((task)->tss.cs),						\
			"g"((task)->tss.eip));						\
	}while(0)

#define setup_syscall_stack(task)\
	do {												\
		asm volatile("									\
			movl %0, 40(%%ebp)			;				\
			movw %1, 36(%%ebp)			;				\
			movl %2, 32(%%ebp)			;				\
			movw $0x18, %%ax 			;				\
			movw %%ax, %%ds				;				\
			"::"g"((task)->tss.eflags),				\
			"g"((task)->tss.cs),					\
			"g"((task)->tss.eip));					\
	}while(0)
	
#define save_task_state(task)				\
		do {								\
			asm volatile("					\
				movw 48(%%ebp), %0;			\
				movl 44(%%ebp), %1;			\
				movl 40(%%ebp), %2;			\
				movw 36(%%ebp), %3;			\
				movl 32(%%ebp), %4;			\
				":"=g"((task)->tss.ss), 	\
				"=g"((task)->tss.esp), 		\
				"=g"((task)->tss.eflags),	\
				"=g"((task)->tss.cs),		\
				"=g"((task)->tss.eip));		\
		}while(0)
		
#define save_syscall_state(task)				\
		do {									\
			asm volatile("						\
				movl 40(%%ebp), %0;				\
				movw 36(%%ebp), %1;				\
				movl 32(%%ebp), %2;				\
				":"=g"((task)->tss.eflags),	\
				"=g"((task)->tss.cs),		\
				"=g"((task)->tss.eip));		\
		}while(0)


 /*
 *int32_t pit_init()
 *DESCRIPTION: Initializes the pit by writing the data to the specified port 0x43,
               also sets the pit's frequency to 100 and enables it
 *
 *INPUTS: none
 *OUTPUTS: returns 0
 *SIDE EFFECTS: initializes the pit for use
 */		
int32_t pit_init()
{	
	uint8_t port, data;
	
	port = PIT_COMMAND;
	data = ACCESS_MODE_LOHIBYTE | OPERATING_MODE_3;
	outb (data, port);
	pit_set_frequency(INIT_FREQUENCY);
	pit_enable();
	
	return 0;
}

 /*
 *int32_t pit_set_frequency(uint32_t frequency)
 *DESCRIPTION: Sets the frequency by dividing the PIT_FREQUENCY by 
               the input frequency, and writes to port 0x40
 *
 *INPUTS: frequency
 *OUTPUTS: returns 0
 *SIDE EFFECTS: sets the pits frequency to the specified
 */
int32_t pit_set_frequency(uint32_t frequency)
{
	uint8_t dh, dl;
	unsigned long flags;
	cli_and_save(flags);
	if (frequency > PIT_MAX_FREQ || frequency < PIT_MIN_FREQ)
	{
		restore_flags(flags);
		return -1;
	}
	dh = (PIT_FREQUENCY / frequency) >> 8;
	dl = (PIT_FREQUENCY / frequency);
	outb (dl, PIT_CHANNEL_0);
	outb (dh, PIT_CHANNEL_0);
	restore_flags(flags);
	return 0;
}

 /*
 *void pit_process_interrupt()
 *DESCRIPTION: processes the interrupt depending on if it interrupt a user program of a system call
 *
 *INPUTS: none
 *OUTPUTS: none
 *SIDE EFFECTS: updates state of task to running or to system call depending on 
 *              if a user program or system call is interrupted
 */
void pit_process_interrupt()
{
	task_t * cur_task = get_cur_task();	
	uint16_t cs;
	get_cs(cs);	

	//If we interrupted a user program
	if (cs == USER_CS)
	{
		//Update the state of task to running
		set_cur_task_state(TASK_RUNNING);
		save_task_state(cur_task);
	}
	
	//If we interrupted a system call
	else if (cs == KERNEL_CS)
	{
		//Update the state of task to a system call
		set_cur_task_state(TASK_SYS_CALL);
		save_syscall_state(cur_task);
	}
	
	cur_task = switch_task(cur_task->pid, get_next_task()->pid);	
	if (cur_task->state == TASK_RUNNING)
		setup_task_stack(cur_task);
	else
		setup_syscall_stack(cur_task);
}
 /*
 *void pit_enable()
 *DESCRIPTION: enables the pit
 *
 *INPUTS: none
 *OUTPUTS: none
 *SIDE EFFECTS: enables the pit
 */
void pit_enable()
{
	enable_irq(0);
}

 /*
 *void pit_disable()
 *DESCRIPTION: disables the pit
 *
 *INPUTS: none
 *OUTPUTS: none
 *SIDE EFFECTS: disables the pit
 */
void pit_disable()
{
	disable_irq(0);
}
