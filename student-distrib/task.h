#ifndef TASK__H
#define TASK__H

#include "types.h"
#include "filesys.h"
#include "x86_desc.h"

//To-Do: Fill out rest of struct
typedef struct task_t{
	struct task_t * parent_task; //Parent
	struct task_t * child_task; //Children
	struct task_t * sibling_task; //Siblings

	uint8_t pName[32];
	int32_t pid; //PID
	file_t files[8]; //File array
	tss_t tss;//TSS
	//States (Task Running, Task Stopped, Interruptible, Uninterruptible, Should be halted)
	
	//Signals
	
	//Scheduling Statistics
	
	//Starting arguments
	uint8_t args[128];
} task_t;

int32_t setup_task_switch(task_t * old_task, task_t * new_task);
task_t * init_task(uint32_t pid);
task_t * get_task(uint32_t pid);
int32_t save_state(task_t * task);
int32_t load_state(task_t * task);
int32_t load_program_to_task(task_t * task, uint32_t addr, const uint8_t * fname, const uint8_t args [128]);

#endif
