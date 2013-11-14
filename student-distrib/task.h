#ifndef TASK__H
#define TASK__H

#include "types.h"
#incldue "filesys.h"

//To-Do: Fill out rest of struct
typedef struct task_t{
	struct task_t * parent_task; //Parent
	struct task_t * sibling_task; //Siblings

	uint8_t pid; //PID
	file_t files[8]; //File array
	tss_t tss;//TSS
	//States (Task Running, Task Stopped, Interruptible, Uninterruptible, Should be halted)
	
	//Signals
	
	//Scheduling Statistics
} task_t;

#endif