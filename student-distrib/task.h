#ifndef TASK__H
#define TASK__H

#include "lib.h"
#include "types.h"
#include "filesys.h"
#include "x86_desc.h"

//Task state machine
typedef enum task_state {
	TASK_RUNNING = 0,
	TASK_SYS_CALL,
	TASK_EXCEPTION
} task_state;

typedef struct task_t{
	struct task_t * parent_task; //Parent

	uint8_t pName[32]; //Human-Readable process name
	int32_t pid; //Process ID
	int32_t ptid; //Process Tree ID
	file_t files[8]; //File array
	tss_t tss; //TSS
	
	
	//These are the return values for when a program halts
	uint32_t ret_ebp;
	uint32_t ret_esp;
	uint32_t ret_eip;
	uint32_t ret_eflags;
	
	//These variables hold the pointer to the page directory and page table
	uint32_t * page_directory;
	uint32_t * page_table;
	
	//States (Task Running, Task Stopped, Interruptible, Uninterruptible, Should be halted)
	task_state state;
	
	//Starting arguments
	uint8_t args[128];
} task_t;

task_t * init_task(int32_t pid);
task_t * get_task(int32_t pid);
uint32_t get_task_addr(uint32_t pid);
uint32_t get_task_stack_addr(uint32_t pid);
void save_state(task_t * task, uint32_t esp, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
uint32_t load_state(task_t * task, uint16_t cs, uint32_t esp, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
int32_t load_tss(task_t * task);
int32_t load_program_to_task(task_t * task, uint32_t addr, const uint8_t * fname, const uint8_t args [128]);

#endif
