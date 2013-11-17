#include "sched.h"
#include "paging.h"
#include "filesys.h"

#define EXECUTION_ADDR 0x8000000

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
	
#define invtlb(task)										\
		do {											\
		asm volatile("									\
			movl %cr3, %eax \n							\
			movl %eax, %cr3");						\
	}while(0)
	
#define setup_task_stack(task)							\
	do {												\
		asm volatile("									\
			pushl %0 \n									\
			pushl %1 \n									\
			pushl %2 \n									\
			pushl %3 \n									\
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
			":"=r"((task)->tss.esp),					\
			  "=r"((task)->tss.ebp));					\
	}while(0)
	
#define load_task_state(task)							\
		do {											\
		asm volatile("									\
			movl %0, %%esp								\
			"::"r"((task)->tss.esp));					\
	}while(0)
	
#define setup_return_stack(task)							\
	do {													\
		asm volatile("										\
			pushl %0 \n										\
			pushl %1 \n										\
			pushl %2 \n										\
			pushl %3 \n										\
			pushl %%eip \n									\
			movw $0x2b, %%ax \n								\
			movw %%ax, %%ds									\
			"::"r"((task)->tss.ss), 						\
			"r"((task)->tss.esp), 							\
			"r"((task)->tss.eflags),						\
			"r"((task)->tss.cs));							\
	}while(0)
		
sched_t schedular = {
	.task_vector = 0,
	.max_tasks = 6,
	.num_tasks = 0,
	.cur_task = -1
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

	get_cr3(pd);
	map_page_directory(addr, addr, 1, 1);
	//Set up user-space memory
	//pd[2 + pid] = (((unsigned int) addr) & 0xFFC00000);
	//set page as 4MB, set supervisor priv., set r/w, and present
	//pd[2 + pid] |=  (0x87);  
	
	//Load the file image
	task = init_task(pid);
	if (schedular.cur_task != -1)
		task->parent_task = get_task(schedular.cur_task);
		
	if (load_program_to_task(task, addr + 0x48000, fname, args) == -1)
	{
		clear_pid(pid);
		return -1;
	}
	++schedular.num_tasks;
	
	return pid;
}

int32_t end_task(int32_t pid)
{
	uint32_t addr;
	task_t * parent_task = get_task(pid)->parent_task;
	
	//Map the file image into memory
	set_task_cr3(parent_task);
	get_cr3(pd);
	
	addr = 0x800000 + (pid * 0x400000);
	clear_pid(pid);
	clear_pde(addr);
	
	//pd[(addr & 0xFFC00000) >> 22] = 0;
	--schedular.num_tasks;
	
	set_cur_task(parent_task->pid);
	return 0;
}

task_t * get_cur_task()
{
	return schedular.cur_task < 0 ? NULL : get_task(schedular.cur_task );
}

int32_t set_cur_task(int32_t pid)
{
	//uint32_t addr;
	
	schedular.cur_task = pid;
	//addr = 0x800000 + (pid * 0x400000);
	//map_page_directory(addr, EXECUTION_ADDR, 1, 1);
	
	return 0;
}

int32_t switch_task(int32_t pid)
{
	int32_t ret = 0;
	task_t * old_task; 
	task_t * new_task;
	
	old_task = get_cur_task();
	new_task = get_task(pid);
	
	printf("\n======== Now Running: %s(%d) ========\n", new_task->pName, pid);
	
	old_task->tss.eip = (uint32_t)(&&halt_addr);
	
	set_cur_task(pid);
	save_task_state(old_task);
	load_tss(new_task);
	setup_task_stack(new_task);
	set_task_cr3(new_task);
	iret();

halt_addr:
	asm volatile("movb %%al, %0;":"=g"(ret));
	if ((int8_t)ret == -1)
		ret = -1;
	printf("======== Exited %s(%d) with %d status ========\n", new_task->pName, pid, ret);
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
	
	init_task->tss.cs = 0x0010;
	init_task->tss.ds = 0x0018;
	init_task->tss.ss = 0x0018;
	init_task->tss.ldt_segment_selector = KERNEL_LDT;
	
	return 0;
}
