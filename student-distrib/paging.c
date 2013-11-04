#include "paging.h"
#include "lib.h"
#include "filesys.h"

#define FIRST_20_BITS 0xFFFFF000 /*first 20 bits of 32 bit value used for mask*/
#define FIRST_10_BITS 0xFFC00000 /*first 10 bits of 32 bit value used for mask*/
#define KERNEL_LOCATION   0x400000 /*starting location in memory where kernel resides*/
#define NUM_PAGES 1024 /*number of pages in a page directory*/
#define SIZE_4KB_PAGE 0x1000 /*this size of a 4kb page (4KB!)*/
#define CR4_4MB_PAGE_BIT 0x10 /*bit of CR4 control register that enables 4mb paging*/
#define CR0_PAGE_ENBL_BIT 0x80000000 /*bit of CR0 control register that enables paging*/
#define SUPERVISOR_RW_PRESENT 0x3 /*bits necessary to set supervisor to priv., r/w adn pressent*/
#define PAGE_SIZE_BIT 0x80 /*bit used to set page to 4MB */

extern unsigned int page_directory;
extern unsigned int page_table;

static unsigned int * pd = &page_directory;
static unsigned int * pt = &page_table;
static unsigned int * kernel_page = (unsigned int *) KERNEL_LOCATION;

/* void paging_init()
 *   DESCRIPTION: Initializes paging for the CPU. The first 4MB are used with 4KB paging,
 *	 while from 4MB onwards, the pages are of size 4MB. Uses the global page_directory and page_table
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: page faults can now be generated, control registers cr0, cr3, cr4 modified
 */
void paging_init(){
	int i;
	unsigned int temp, addr;
		
	//sets up 4KB page directory for video memory and puts 20 MSB of address in
	pd[0] = (((unsigned int) pt) & FIRST_20_BITS);
	//set page as 4KB, set supervisor priv., set r/w, and present
	pd[0] |= SUPERVISOR_RW_PRESENT; 
	
	addr = 0;
	for ( i = 0; i < NUM_PAGES; i ++)
	{
		//Set up page table base address
		//Set the first entry to not be present for seg faults (previously set off in x86_desc)
		if (i != 0)
			pt[i] = addr | SUPERVISOR_RW_PRESENT; //set page as 4KB, set supervisor priv., set r/w, and present
		addr += SIZE_4KB_PAGE;//4KBytes added to get to the next 4kb page address;
	}
	
	//sets up 4MB page for kernel and puts proper dir. address in
	pd[1] = (((unsigned int) kernel_page) & FIRST_10_BITS);
	//set page as 4MB, set supervisor priv., set r/w, and present
	pd[1] |=  ( PAGE_SIZE_BIT | SUPERVISOR_RW_PRESENT);  
	
	//Load the address of the page directory from pd[0] into cr3
	asm volatile("movl %0, %%cr3" 
						:
						: "r"(pd) );
				  
	//gets current bits of cr4 and puts into temp
	asm volatile("movl %%cr4, %0"
						: "=r"(temp));
	//enables 4MB paging, all other bits should remain the same
	temp |= CR4_4MB_PAGE_BIT;
	//stores back into cr4 to actually enable 4MB paging
	asm volatile("movl %0, %%cr4"
						:
						: "r"(temp));
	

	//gets current bits of cr0 and puts into temp
	asm volatile("movl %%cr0, %0"
						: "=r"(temp));
	//turns on bit so that when in register, paging will be enabled
	temp |= CR0_PAGE_ENBL_BIT;
	//Store back into cr0 to actually enable paging
	asm volatile("movl %0, %%cr0"
						:
						: "r"(temp));
}


