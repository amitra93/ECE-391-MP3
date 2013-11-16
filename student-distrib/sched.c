#include "sched.h"
#include "paging.h"
#include "filesys.h"

#define EXECUTION_ADDR 0x8000000

#define iret()					\
	do { 						\
		asm volatile("iret");	\
	}while(0)

#define setup_task_stack(task) 								\
	do {													\
		asm volatile("										\
			pushl %0 \n										\
			pushl %1 \n										\
			pushl %2 \n										\
			pushl %3 \n										\
			pushl %4"::"r"((task)->tss.ss), 				\
			"r"((task)->tss.esp), 							\
			"r"((task)->tss.eflags),						\
			"r"((task)->tss.cs),							\
			"r"((task)->tss.eip));							\
	}while(0)
		
sched_t schedular = {
	.task_vector = 0,
	.max_tasks = 6,
	.num_tasks = 0,
	.cur_task = -1
};
	
static int32_t clear_pid(uint32_t pid)
{
	schedular.task_vector &= ~(1 << pid);
	return 0;
}

static int32_t get_new_pid()
{
	uint32_t i;
	for (i = 0; i < schedular.max_tasks; i ++)
	{
		if ( (schedular.task_vector & (1 << i)) == 0)
		{
			schedular.task_vector |= 1 << i;
			return i;
		}
	}
	return -1;
}

int32_t create_task(const uint8_t * fname, const uint8_t * args)
{
	task_t * task;
	int32_t pid;
	uint32_t addr;
	
	//Get a new pid
	if ( (pid = get_new_pid()) < 0)
		return -1;
		
	//Map the file image into memory
	addr = 0x800000 + (pid * 0x400000);
	map_page_directory(addr, addr, 1, 1);
	
	//Load the file image
	task = init_task(pid);
	if (schedular.cur_task != -1)
		task->parent_task = get_task(schedular.cur_task);
		
	if (load_program_to_task(task, addr + 0x48000, fname, args) == -1)
	{
		clear_pid(pid);
		return -1;
	}
	task->tss.esp = addr + 0x400000 - 4;
	++schedular.num_tasks;
	
	return pid;
}

int32_t end_task(uint32_t pid)
{
	uint32_t addr;
		
	//Map the file image into memory
	addr = 0x800000 + (pid * 0x400000);
	clear_pte(addr);
	clear_pid(pid);
	--schedular.num_tasks;
	
	return 0;
}

task_t * get_cur_task()
{
	return schedular.cur_task < 0 ? NULL : get_task(schedular.cur_task );
}

int32_t set_cur_task(uint32_t pid)
{
	uint32_t addr;
	
	schedular.cur_task = pid;
	addr = 0x800000 + (pid * 0x400000);
	map_page_directory(addr, EXECUTION_ADDR, 1, 1);
	
	return 0;
}

int32_t switch_task(uint32_t pid)
{
	task_t * old_task = NULL; 
	task_t * new_task;
	
	if (schedular.cur_task != -1)
		old_task = get_task(schedular.cur_task);
	set_cur_task(pid);
	new_task = get_task(pid);
	setup_task_switch(old_task, new_task);
	setup_task_stack(new_task);
	iret();
	
	return 0;
}
