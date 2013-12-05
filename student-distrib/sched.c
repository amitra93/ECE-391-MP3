#include "sched.h"
#include "paging.h"
#include "filesys.h"
#include "lib.h"

#define DEBUG		
#define EXECUTION_ADDR 0x8000000
#define PROGRAM_IMAGE 0x48000

#define iret()					\
	do { 						\
		asm volatile("iret");	\
	}while(0)

#define get_cr3(cr3)											\
	do {														\
		asm volatile("											\
			movl %%cr3, %0":"=r"(cr3));							\
	}while(0)
	
#define set_task_cr3(task)										\
		do {													\
		asm volatile("											\
			movl %0, %%eax \n									\
			movl %%eax, %%cr3"::"r"((task)->page_directory));	\
	}while(0)
	
#define setup_task_stack(task)							\
	do {												\
		asm volatile("									\
			pushw $0 \n									\
			pushw %0 \n									\
			pushl %1 \n									\
			pushl %2 \n									\
			pushw $0 \n									\
			pushw %3 \n									\
			pushl %4 \n									\
			movw $0x2b, %%ax \n							\
			movw %%ax, %%ds								\
			"::"r"((task)->tss.ss), 					\
			"r"((task)->tss.esp), 						\
			"r"((task)->tss.eflags),					\
			"r"((task)->tss.cs),						\
			"r"((task)->tss.eip));						\
	}while(0)
	
#define save_task_state(task)							\
		do {											\
		asm volatile("									\
			movl %%esp, %0	\n							\
			movl %%ebp, %1								\
			":"=r"((task)->ret_esp),					\
			  "=r"((task)->ret_ebp));					\
	}while(0)
	
sched_t schedular = {
	.task_vector = 0,
	.max_tasks = 7,
	.num_tasks = 0,
	.cur_task = -1,
	
	.ptree_tasks = {[0 ... 31] = -1},
	.ptree_vector = 0,
	.max_ptrees = 3,
	.num_ptrees = 0,
	.cur_ptree = -1,
};
	
static int32_t clear_pid(int32_t pid)
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

static int32_t clear_ptid(int32_t ptid)
{
	schedular.ptree_tasks[ptid] = -1;
	schedular.ptree_vector &= ~(1 << ptid);
	return 0;
}

static int32_t get_new_ptid()
{
	uint32_t i;
	for (i = 0; i < schedular.max_ptrees; i ++)
	{
		if ( (schedular.ptree_vector & (1 << i)) == 0)
		{
			schedular.ptree_vector |= 1 << i;
			return i;
		}
	}
	return -1;
}

int32_t create_ptree ()
{
	int32_t ptid;
	if ((ptid = get_new_ptid()) < 0)
		return -1;
	++schedular.num_ptrees;
	schedular.ptree_tasks[ptid] = -1;
	return ptid;
}

int32_t create_task(const uint8_t * fname, const uint8_t * args)
{
	task_t * task;
	int32_t pid;
	uint32_t addr;
	unsigned long flags;
	cli_and_save(flags);
	//Get a new pid
	if ( (pid = get_new_pid()) < 0)
		return -1;
	
	//Map the file image into memory
	addr = get_task_addr(pid);

	//Set up user-space memory
	get_cr3(pd);
	map_page_directory(addr, addr, 1, 1);
	
	//Load the file image
	task = init_task(pid);
	if (schedular.cur_task != -1)
	{
		task->parent_task = get_task(schedular.cur_task);
		task->ptid = task->parent_task->ptid;
	}
		
	if (load_program_to_task(task, addr + PROGRAM_IMAGE, fname, args) == -1)
	{
		clear_pid(pid);
		restore_flags(flags);
		return -1;
	}
	
	++schedular.num_tasks;
	restore_flags(flags);
	return pid;
}

int32_t end_task(int32_t pid)
{
	uint32_t addr;
	unsigned long flags;
	cli_and_save(flags);
	task_t * parent_task = get_task(pid)->parent_task;
	
	//Map the file image into memory
	set_task_cr3(parent_task);
	get_cr3(pd);
	
	addr = get_task_addr(pid);
	clear_pid(pid);
	clear_pde(addr);
	
	--schedular.num_tasks;
	set_ptree_task(parent_task->ptid, parent_task->pid);
	set_cur_task(parent_task->pid);
	restore_flags(flags);
	return 0;
}

task_t * get_cur_task()
{
	return schedular.cur_task < 0 ? NULL : get_task(schedular.cur_task );
}

task_t * get_next_task()
{
	uint32_t i;
	for (i = schedular.cur_ptree; i < schedular.max_ptrees; i ++)
	{
		if ( (schedular.ptree_vector & (1 << i)) > 0)
			return get_task(schedular.ptree_tasks[i]);
	}
	
	for (i = 0; i < schedular.cur_ptree; i ++)
	{
		if ( (schedular.ptree_vector & (1 << i)) > 0)
			return get_task(schedular.ptree_tasks[i]);
	}
	
	//If there are no other active tasks, return current
	return get_cur_task();
}

task_state get_cur_task_state()
{
	return get_cur_task()->state;
}

int32_t set_cur_task(int32_t pid)
{
	schedular.cur_task = pid;	
	schedular.cur_ptree = get_task(pid)->ptid;
	return 0;
}

int32_t set_ptree_task(int32_t ptid, int32_t pid)
{
	schedular.ptree_tasks[ptid] = pid;
	return 0;
}

int32_t clear_ptree_task(int32_t ptid)
{
	schedular.ptree_tasks[ptid] = -1;
	return 0;
}

int32_t set_cur_task_state(task_state state)
{
	get_cur_task()->state = state;
	return 0;
}

task_t * switch_task(int32_t old_pid, int32_t new_pid)
{
	task_t * new_task = get_task(new_pid);
	
	//Map the old process' video memory to garbage
	map_page_directory(GARBAGE_VID_MEM, VIRTUAL_VID_MEM, 1, 1);
	
	set_cur_task(new_pid);
	load_tss(new_task);
	set_task_cr3(new_task);
	get_cr3(pd);
	
	//Map the new process' video memory to the real deal
	map_page_directory(VIDEO, VIRTUAL_VID_MEM, 1, 1);
	
	return new_task;
}

int32_t execute_task(int32_t pid)
{
	int32_t ret = 0;
	task_t * old_task; 
	task_t * new_task;
	
	old_task = get_cur_task();
	new_task = get_task(pid);
	new_task->state = TASK_RUNNING;

	old_task->ret_eip = (uint32_t)(&&halt_addr);
	
	set_cur_task(pid);
	set_ptree_task(new_task->ptid, pid);
	
	save_task_state(old_task);
	load_tss(new_task);
	setup_task_stack(new_task);
	set_task_cr3(new_task);
	get_cr3(pd);
	iret();

halt_addr:
	asm volatile("movb %%al, %0;":"=m"(ret));
	if (get_cur_task_state() == TASK_EXCEPTION)
		ret = 256;
		
	end_task(get_cur_task()->pid);
	load_tss(get_cur_task());
	return ret;
}

int32_t tasks_init()
{		
	task_t * init_task;
	uint8_t fname [32] = "shell";
	uint8_t args[128] = "";
	
	create_task(fname, args);
	set_cur_task(0);
	
	init_task = get_task(0);
	init_task->ptid = create_ptree();
	set_ptree_task(init_task->ptid, init_task->pid);
	
	init_task->tss.cs = 0x0010;
	init_task->tss.ds = 0x0018;
	init_task->tss.ss = 0x0018;
	init_task->tss.ldt_segment_selector = KERNEL_LDT;
	
	return 0;
}
