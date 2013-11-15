#include "paging.h"
#include "types.h"
#include "x86_desc.h"

#define INIT_TASK_ADDR 0x800000

//To-Do: Set file[0]=stdin and file[1]=stdout
//To-Do: Initialize the TSS
static int32_t init_task(uint8_t pid)
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
	task->tss.esp = 0x400000 - 2;
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
	{
		args[i] = 0;
	}
}


//To-DO: Fill these guys out
//Save the task's state
void save_state(task_t * task) { }
//Load the task's state
void load_state(task_t * task){ }

//Loads task's tss into TSS
void load_tss(task_t * task)
{
	tss.esp0 = task->esp0;
	tss.ss0 = task->ss0;
}


int32_t init_tasks()
{
	task_t * task;
	
	//Map the running task into memory
	map_page_directory(INIT_TASK_ADDR, 0x8000000, 1, 1);
	init_task(0);
}

int32_t setup_task_switch(task_t * task)
{
	if (task == NULL)
		return -1;
	

}