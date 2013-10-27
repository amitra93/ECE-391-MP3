#include "paging.h"
#include "lib.h"

extern unsigned int page_directory;
extern unsigned int page_table;

static unsigned int * pd = &page_directory;
static unsigned int * pt = &page_table;

static unsigned int * kernel_page = 0x400000;

void paging_init(){
	int i;
	unsigned int temp, addr;
		
	//sets up 4KB page table for kernel and puts proper table address in
	pd[0] = (((unsigned int) pt) & 0xFFFFF000);
	//set page as 4KB, set supervisor priv., set r/w, and present
	pd[0] |= 0x3;//might also want to make global, will check later  
	
	addr = 0;
	for ( i = 0; i < 1024; i ++)
	{
		//Set up page table base address
		pt[i] = addr | 0x3; //set page as 4KB, set supervisor priv., set r/w, and present
		addr += 0x1000;//4096;
	}
	
	//sets up 4MB page for kernel and puts proper dir. address in
	pd[1] = (((unsigned int) kernel_page) & 0xFFC00000);
	//set page as 4MB, set supervisor priv., set r/w, and present
	pd[1] |= 0x83;//might also want to make global, will check later  

	//Load the address of the page directory
	asm volatile("movl %0, %%cr3" :: "r"(pd) );
	
	/*asm volatile("movl $0x8, %%eax;\
			  mov %%cr4, %%ebx;\
			  or %%eax, %%ebx;\
			  mov %%ebx, %%cr4;":::"eax","ebx");
	
	//enables paging
	asm volatile("movl $1, %%eax;\
				  shll $31, %%eax;\
				  mov %%cr0, %%ebx;\
				  or %%eax, %%ebx;\
				  mov %%ebx, %%cr0;":::"eax","ebx");*/
				  
	//enables 4MB paging 
	asm volatile("movl %%cr4, %0": "=b"(temp));
	temp |= 0x10;
	asm volatile("movl %0, %%cr4":: "b"(temp));
				  
	asm volatile("movl %%cr0, %0": "=b"(temp));
	temp |= 0x80000000;
	asm volatile("movl %0, %%cr0":: "b"(temp));



}


