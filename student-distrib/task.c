#include "paging.h"
#include "types.h"
#include "x86_desc.h"
#include "task.h"

#define INIT_TASK_ADDR 0x800000

extern unsigned int page_directory;
static unsigned int * pd = &page_directory;

//To-Do: Set file[0]=stdin and file[1]=stdout
task_t * init_task(uint32_t pid)
{
	task_t * task;
	uint32_t stack_addr;
	uint8_t i;
	
	/*
	* DEBUG MESSAGE:
	* The esp's might be incorrect. I don't know if they point directly to the last byte, 
	* or if they point to the byte right before it.
	*/
	
	stack_addr = 0x400000 - (0x2000*pid) - 2;
	task = (task_t*)(stack_addr & 0x3FE000);
	
	task->tss.esp0 = stack_addr;
	task->tss.ss0 = KERNEL_DS;
	task->tss.cs = USER_CS;
	task->tss.ds = USER_DS;
	task->tss.cr3 = (uint32_t)pd;
	task->tss.eip = 0; //Set to this to induce a page fault if no program loaded
	task->tss.ldt_segment_selector = KERNEL_LDT;
	
	task->parent_task = NULL;
	task->child_task = NULL;
	task->sibling_task = NULL;
	task->pid = pid;	
	
	for (i = 0; i < 8; i ++)
	{
		task->files[i].fops = NULL;
		task->files[i].inode = NULL;
		task->files[i].offset = 0;
		task->files[i].in_use = 0;
	}
	for (i = 0; i < 128; i ++)
		task->args[i] = 0;
	
	return task;
}

//To-DO: Fill these guys out
//Save the task's state
int32_t save_state(task_t * task) { return 0; }
//Load the task's state
int32_t load_state(task_t * task) { return 0; }

//Loads task's tss into TSS
static void load_tss(task_t * task)
{
	tss.esp0 = task->tss.esp0;
	tss.ss0 = task->tss.ss0;
}

task_t * get_task(uint32_t pid)
{
	return (task_t*)((0x400000 - (0x2000*pid) - 2) & 0x3FE000);
}

int32_t setup_task_switch(task_t * old_task, task_t * new_task)
{
	if (old_task != NULL)
		save_state(old_task);
	load_state(new_task);
	load_tss(new_task);
	return 0;
}

int32_t load_program_to_task(task_t * task, uint32_t addr, const uint8_t * fname, const uint8_t * args)
{
	uint32_t i;
	int32_t exe_addr;
	if ((exe_addr = load_program(fname, (uint8_t*)addr)) == -1 || task == NULL)
		return -1;
	task->tss.eip = exe_addr;
	
	for ( i = 0; i < 128; i ++)
		task->args[i] = args[i];
	return 0;
}

