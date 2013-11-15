#include "sched.h"
#include "paging.h"
#include "filesys.h"

#define EXECUTION_ADDR 0x8000000

static int32_t clear_pid(uint32_t pid)
{
	schedular.task_vector &= ~(1 << pid);
}

static int32_t get_new_pid()
{
	uint32_t i;
	for (i = 0; i < schedular.max_tasks; i ++)
	{
		if (schedular.task_vector & (1 << i) == 0)
			return i;
	}
	return -1;
}

int32_t create_task(const uint8_t * fname, const uint8_t * args)
{
	task_t * task;
	task_t * parent;
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
		
	load_program_to_task(task, addr, fname, args);
	
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

int32_t set_cur_task(uint32_t pid)
{
	schedular.cur_task = pid;
	addr = 0x800000 + (pid * 0x400000);
	map_page_directory(addr, EXECUTION_ADDR, 1, 1);
	
	return 0;
}
