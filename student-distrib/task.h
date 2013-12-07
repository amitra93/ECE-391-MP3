#ifndef TASK__H
#define TASK__H

#include "lib.h"
#include "types.h"
#include "filesys.h"
#include "x86_desc.h"

typedef enum task_state {
	TASK_RUNNING = 0,
	TASK_STOPPED,
	TASK_PREEMPT,
	TASK_SUICIDE,
	TASK_EXCEPTION,
	TASK_INTERRUPTIBLE,
	TASK_UNINTERRUPTIBLE,
	TASK_SYS_CALL
} task_state;

//To-Do: Fill out rest of struct
typedef struct task_t{
	struct task_t * parent_task; //Parent
	struct task_t * child_task; //Children
	struct task_t * sibling_task; //Siblings

	uint8_t pName[32];
	int32_t pid; //Process ID
	int32_t ptid; //Process Tree ID
	file_t files[8]; //File array
	tss_t tss;//TSS
	tss_t sys_tss; //TSS used in a system call
	
	uint32_t ret_ebp;
	uint32_t ret_esp;
	uint32_t ret_eip;
	
	uint32_t * page_directory;
	uint32_t * page_table;
	uint8_t * video_mem;
	
	//States (Task Running, Task Stopped, Interruptible, Uninterruptible, Should be halted)
	task_state state;
	//Signals
	
	//Scheduling Statistics
	
	//Starting arguments
	uint8_t args[128];
} task_t;

task_t * init_task(int32_t pid);
task_t * get_task(int32_t pid);
uint32_t get_task_addr(uint32_t pid);
uint32_t get_task_stack_addr(uint32_t pid);
void save_state(task_t * task, uint16_t cs, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
uint32_t load_state(task_t * task, uint16_t cs, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi);
int32_t load_tss(task_t * task);
int32_t load_program_to_task(task_t * task, uint32_t addr, const uint8_t * fname, const uint8_t args [128]);

#endif
