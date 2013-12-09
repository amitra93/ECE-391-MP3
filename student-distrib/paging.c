#include "paging.h"
#include "lib.h"
#include "filesys.h"

unsigned int * pd = &page_directory;
unsigned int * pt = &page_table;

static unsigned int * kernel_page = (unsigned int *) KERNEL_LOCATION;

/* void paging_init()
 *   DESCRIPTION: Initializes paging for the CPU. The first 4MB are used with 
				  4KB paging,
 *	              while from 4MB onwards, the pages are of size 4MB. Uses the 
				  global page_directory  and page_table
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: page faults can now be generated, control registers cr0, cr3,
			       cr4 modified
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
	
	map_page_directory(VIDEO, VIRTUAL_VID_MEM, 1, 1);
	
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
 /*
 *int32_t map_page_directory(uint32_t phys_addr, uint32_t virt_addr, 
                             uint8_t size, uint8_t user)
 *DESCRIPTION: This function maps the page directory
 *
 *INPUTS: phys_addr, virt_addr, size, user
 *OUTPUTS: returns 0
 *SIDE EFFECTS: page directory
 */
//size = 0 when 4kb size = 1 when 4mb
int32_t map_page_directory(uint32_t phys_addr, uint32_t virt_addr, uint8_t size, uint8_t user)
{	
	//4KB Page
	if (size == 0)
	{
		uint32_t pd_index = (virt_addr & FIRST_10_BITS) >> 22;
		pd[pd_index] = (phys_addr & FIRST_20_BITS) | (user << 2) |  RW_PRESENT;
	}
	else if (size == 1)
	{
		uint32_t pd_index = (virt_addr & FIRST_10_BITS) >> 22;
		pd[pd_index] = (phys_addr & FIRST_10_BITS) | PAGE_SIZE_BIT | (user << 2) |  RW_PRESENT;
	}
	
	return 0;
}

 /*
 *int32_t map_page_table(uint32_t phys_addr, uint32_t virt_addr, uint8_t user)
 *DESCRIPTION: This function maps the page table
 *
 *INPUTS: phys_addr, virt_addr, user
 *OUTPUTS: returns 0
 *SIDE EFFECTS: page table
 */
//size = 0 when 4kb size = 1 when 4mb
int32_t map_page_table(uint32_t phys_addr, uint32_t virt_addr, uint8_t user)
{
	uint32_t pd_index;
	uint32_t pt_index;
	uint32_t * pg_table;
		
	pd_index = (virt_addr & FIRST_10_BITS) >> 22;
	pt_index = (virt_addr & 0x3FF000) >> 12;
	
	pg_table = (uint32_t *)(pd[pd_index] & FIRST_20_BITS);
	pg_table[pt_index] = (phys_addr & FIRST_20_BITS) | (user << 2) |  RW_PRESENT;
	return 0;
}

 /*
 *int32_t map_page_table_from_index(uint32_t phys_addr, uint32_t pd_index,
                                    uint32_t pt_index, uint8_t user)
 *DESCRIPTION: This function maps the page table from the index
 *
 *INPUTS: phys_addr, pd_index, pt_index, user
 *OUTPUTS: returns 0
 *SIDE EFFECTS: none
 */
int32_t map_page_table_from_index(uint32_t phys_addr, uint32_t pd_index, uint32_t pt_index, uint8_t user)
{
	uint32_t * pg_table;
	
	pg_table = (uint32_t *)(pd[pd_index] & FIRST_10_BITS);
	pg_table[pt_index] = (phys_addr & FIRST_20_BITS) | (user << 2) |  RW_PRESENT;
	
	return 0;
}
 /*
 *int32_t set_pde(uint32_t virt_addr)
 *DESCRIPTION: This function sets the page directory entries
 *
 *INPUTS: virt_addr
 *OUTPUTS: returns 0
 *SIDE EFFECTS: none
 */
int32_t set_pde(uint32_t virt_addr)
{
	uint32_t pd_index = (virt_addr & FIRST_10_BITS) >> 22;
	pd[pd_index] |=  1;
	
	return 0;
}

 /*
 *int32_t set_pte(uint32_t virt_addr)
 *DESCRIPTION: This function sets the page table entries
 *
 *INPUTS: virt_addr
 *OUTPUTS: returns 0
 *SIDE EFFECTS: page table entries are set
 */ 
int32_t set_pte(uint32_t virt_addr)
{
	uint32_t pd_index;
	uint32_t pt_index;
	uint32_t * pg_table;
		
	pd_index = (virt_addr & FIRST_10_BITS) >> 22;
	pt_index = (virt_addr & 0x3FF000) >> 12;
	
	pg_table = (uint32_t *)(pd[pd_index] & FIRST_20_BITS);
	pg_table[pt_index] |= 1;
	return 0;
}

 /*
 *int32_t clear_pde(uint32_t virt_addr)
 *DESCRIPTION: This function clears the page directory entries based on the virtual
			   address
 *
 *INPUTS: virt_addr
 *OUTPUTS: returns 0
 *SIDE EFFECTS: clears the page directory entry
 */ 
int32_t clear_pde(uint32_t virt_addr)
{
	uint32_t pd_index = (virt_addr & FIRST_10_BITS) >> 22;
	pd[pd_index] = 0;
	return 0;
}

/*
 *int32_t clear_pte(uint32_t virt_addr)
 *DESCRIPTION: This function clears the page table entry based on the given 
			   virtual address
 *
 *INPUTS: virt_addr
 *OUTPUTS: returns 0
 *SIDE EFFECTS: clears page table entry
 */
int32_t clear_pte(uint32_t virt_addr)
{
	uint32_t pd_index;
	uint32_t pt_index;
	uint32_t * pg_table;
		
	pd_index = (virt_addr & FIRST_10_BITS) >> 22;
	pt_index = (virt_addr & 0x3FF000) >> 12;
	
	pg_table = (uint32_t *)(pd[pd_index] & FIRST_20_BITS);
	pg_table[pt_index] = 0;
	return 0;
}
