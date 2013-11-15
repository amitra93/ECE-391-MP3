#include "paging.h"
#include "types.h"
#include "x86_desc.h"

#define INIT_TASK_ADDR 0x800000

//To-Do: Set file[0]=stdin and file[1]=stdout
//To-Do: Initialize the TSS
static int32_t init_task(uint32_t phys_addr)
{
	task_t * task;
	uint8_t i;
	
	task = (task_t*)phys_addr;
	task->parent_task = NULL;
	task->child_task = NULL;
	task->sibling_task = NULL;
	task->pid = 0;	
	
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
	
	/*
	* DEBUG MESSAGE:
	* The esp's might be incorrect. I don't know if they point directly to the last byte, 
	* or if they point to the byte right before it.
	*/
	task->tss.esp0 = 0x400000 - (0x2000)*((phys_addr >> 22) - 2) - 2;
	task->tss.ss0 = KERNEL_DS;
	task->tss.cs = USER_CS;
	task->tss.ds = USER_DS;
	task->tss.cr3 = phys_addr;
	task->tss.esp = 0x400000 - 2;
	task->tss.esi = 0xFFFFFFFF; //Set to this much to induce a page fault if no program loaded
	task->tss.ldt_segment_selector = KERNEL_LDT;
	
	*((uint32_t *)(task->tss.esp0 & 0x3FE000)) = *task;
}

void save_regs(task_t * task)
{

}


int32_t init_tasks()
{
	task_t * task;
	
	//Map the first task into memory
	map_page_directory(INIT_TASK_ADDR, 0x8000000, 1, 1);
	init_task(INIT_TASK_ADDR);
}

