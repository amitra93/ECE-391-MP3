#include "paging.h"
#include "types.h"
#include "x86_desc.h"
#include "task.h"
#include "sched.h"
#include "lib.h"

#define KERNEL_STACK_MASK 0x7FE000
#define KERNEL_STACK_SIZE 0x2000
#define INIT_TASK_ADDR   0x800000
#define EXECUTION_ADDR  0x8000000
#define ELAGS_IF 0x200


 /*
 *uint32_t get_task_addr(uint32_t pid)
 *DESCRIPTION: 
 *
 *INPUTS: pid
 *OUTPUTS: returns the task address based on the initial task address of 
		   0x800000 + pid * the size of a 4mb page.
 *SIDE EFFECTS: none
 */
uint32_t get_task_addr(uint32_t pid)
{
	return INIT_TASK_ADDR + (pid * SIZE_4MB_PAGE);
}

 /*
 *uint32_t get_task_stack_addr(uint32_t pid)
 *DESCRIPTION: returns the stack address based on the initial task address 
               of 0x800000 and the kernel's stack size
 *
 *INPUTS: pid
 *OUTPUTS: none
 *SIDE EFFECTS: none
 */
uint32_t get_task_stack_addr(uint32_t pid)
{
	return INIT_TASK_ADDR - (KERNEL_STACK_SIZE * pid) - 4;
}

/*
 *static int32_t init_task_pd(task_t * task)
 *DESCRIPTION: sets up user-space memory, 4mb page for kernel,
			   sets up mapped task memory, video memory
 *
 *INPUTS: task_t * task
 *OUTPUTS: returns 0
 *SIDE EFFECTS: none
 */
static int32_t init_task_pd(task_t * task)
{
	uint32_t i;
	uint32_t *old_pd, *old_pt;
	uint32_t tmp_addr, addr = get_task_addr(task->pid);
	
	task->page_directory = (uint32_t*)(addr);
	task->page_table = (uint32_t*)(addr) + NUM_PAGES;
	task->video_mem = (uint8_t*)(VIRTUAL_VID_MEM) + VIDEO;
	
	old_pd = pd;
	old_pt = pt;
	pd = task->page_directory;
	pt = task->page_table;
	
	pd[0] = (((unsigned int) pt) & FIRST_20_BITS);
	//set page as 4KB, set supervisor priv., set r/w, and present
	pd[0] |= SUPERVISOR_RW_PRESENT; 
	
	tmp_addr = 0;
	for ( i = 0; i < NUM_PAGES; i ++)
	{
		//Set up page table base address
		//Set the first entry to not be present for seg faults (previously set off in x86_desc)
		if (i != 0)
			pt[i] = tmp_addr | SUPERVISOR_RW_PRESENT; //set page as 4KB, set supervisor priv., set r/w, and present
		tmp_addr += SIZE_4KB_PAGE;//4KBytes added to get to the next 4kb page address;
	}
	
	//sets up 4MB page for kernel and puts proper dir. address in
	map_page_directory(KERNEL_LOCATION, KERNEL_LOCATION, 1, 0);
	
	//Set up user-space memory
	map_page_directory(addr, addr, 1, 1);
	
	//Set up mapped task memory
	map_page_directory(addr, EXECUTION_ADDR, 1, 1);

	//Set up video memory
	map_page_directory(VIDEO, VIRTUAL_VID_MEM, 1, 1);
	
	pd = old_pd;
	pt = old_pt;

	return 0;
}

/*
 *task_t * init_task(int32_t pid)
 *DESCRIPTION: initializes the task
 *
 *INPUTS: pid
 *OUTPUTS: returns task
 *SIDE EFFECTS: none
 */
task_t * init_task(int32_t pid)
{
	task_t * task;
	uint32_t stack_addr;
	uint8_t i;
	
	stack_addr = get_task_stack_addr(pid);
	task = (task_t*)(stack_addr & KERNEL_STACK_MASK);
	task->pid = pid;	
	
	init_task_pd(task);
	
	task->tss.esp = EXECUTION_ADDR + SIZE_4MB_PAGE - 4;
	task->tss.ebp = 0;
	task->tss.esp0 = stack_addr;
	task->tss.ss0 = KERNEL_DS;
	task->tss.ss = USER_DS;
	task->tss.cs = USER_CS;
	task->tss.ds = USER_DS;
	task->tss.cr3 = (uint32_t)task->page_directory;
	task->tss.eip = 0; 
	task->tss.ldt_segment_selector = 0;
	task->tss.eflags = ELAGS_IF;
	task->ret_eip = 0xBADF00D;
	task->ret_ebp = 0xBADF00D;
	task->ret_esp = 0xBADF00D;
	task->ret_eflags = 0xBADF00D;
	
	task->parent_task = NULL;
	task->child_task = NULL;
	task->sibling_task = NULL;
	
	for (i = 0; i < 8; i ++)
	{
		task->files[i].fops = NULL;
		task->files[i].inode = NULL;
		task->files[i].offset = 0;
		task->files[i].flags = 0;
	}
	
	//set stdin, stdout
	task->files[0].fops = &term_fops;
	task->files[0].flags = 1;
	task->files[1].fops = &term_fops;
	task->files[1].flags = 1;
	
	for (i = 0; i < 128; i ++)
		task->args[i] = 0;
	
	return task;
}
 /*
 *uint32_t save_state(task_t * task, uint16_t cs, uint32_t esp, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi) 
 *DESCRIPTION: saves the task's state
 *
 *INPUTS: task_t * task, uint16_t cs, uint32_t esp, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi
 *OUTPUTS: none
 *SIDE EFFECTS: saves the task's state
 */
void save_state(task_t * task, uint16_t cs, uint32_t esp, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi) 
{ 
	task->tss.esp = esp+12;
	task->tss.ebp = ebp;
	task->tss.eax = eax;
	task->tss.ebx = ebx;
	task->tss.ecx = ecx;
	task->tss.edx = edx;
	task->tss.esi = esi;
	task->tss.edi = edi;

}

/*
 *uint32_t load_state(task_t * task, uint16_t cs, uint32_t esp, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi) 
 *DESCRIPTION: loads the task's state
 *
 *INPUTS: task_t * task, uint16_t cs, uint32_t esp, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi
 *OUTPUTS: returns 0 if the code segment is user, return 0, else return 1
 *SIDE EFFECTS: loads the state
 */
uint32_t load_state(task_t * task, uint16_t cs, uint32_t esp, uint32_t ebp, uint32_t eax, uint32_t ebx, 
					uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi) 
{
	esp = task->tss.esp;
	ebp = task->tss.ebp;
	eax = task->tss.eax;
	ebx = task->tss.ebx;
	ecx = task->tss.ecx;
	edx = task->tss.edx;
	esi = task->tss.esi;
	edi = task->tss.edi;
	
	if (cs == USER_CS)
		return 0;
	else
		return 1;
}

/*
 *uint32_t load_tss(task_t * task)
 *DESCRIPTION: loads the task's tss into TSS
 *
 *INPUTS: task_t * task
 *OUTPUTS: returns 0
 *SIDE EFFECTS: none
 */
int32_t load_tss(task_t * task)
{
	tss = task->tss;
	
	return 0;
}

/*
 *task_t * get_task(int32_t pid)
 *DESCRIPTION: gets the task
 *
 *INPUTS: pid
 *OUTPUTS: returns a task_t*
 *SIDE EFFECTS: none
 */
task_t * get_task(int32_t pid)
{
	return (task_t*)((INIT_TASK_ADDR - (KERNEL_STACK_SIZE*pid) - 4) & KERNEL_STACK_MASK);
}

/*
 *int32_t load_program_to_task(task_t * task, uint32_t addr, const uint8_t * fname,
							   const uint8_t args[128])
 *DESCRIPTION: loads a program into physical memory
 *
 *INPUTS: task, addr, fname, args[128]
 *OUTPUTS: returns 0
 *SIDE EFFECTS: none
 */
int32_t load_program_to_task(task_t * task, uint32_t addr, const uint8_t * fname, const uint8_t args[128])
{
	uint32_t i;
	int32_t exe_addr;
	if ((exe_addr = load_program(fname, (uint8_t*)addr)) == -1 || task == NULL)
		return -1;
	task->tss.eip = exe_addr;
	
	for ( i = 0; i < 128; i ++)
		task->args[i] = args[i];
		
	for ( i = 0; i < 32; i ++)
		task->pName[i] = fname[i];
	task->pName[31] = 0;
		
	return 0;
}

