#include "paging.h"


unsigned int * pageDirectory = (unsigned int *) 0x800000;
unsigned int * kernelPage    = (unsigned int *) 0x400000;


void paging_init(){
	int i;
	unsigned int temp;
	for(i=0; i<1024; i++){
		pageDirectory[i]=0x2;
	}
	//sets up 4MB page for kernel and puts proper dir. address in
	//pageDirectory[1] = (((unsigned int) kernelPage) & 0xFFC00000);
	//set page as 4MB, set supervisor priv., set r/w, and present
	//pageDirectory[1] |= 0x83;//might also want to make global, will check later  

	asm volatile("movl %0, %%cr3" :: "b"(pageDirectory) );

	//enables paging
	//asm volatile("mov %%cr0, %0": "=b"(temp));
	//temp |= 0x80000000;
	//asm volatile("mov %0, %%cr0":: "b"(temp));

	//enables 4MB paging 
	/*asm volatile("mov %%cr4, %0": "=b"(temp));
	temp |= 0x10;
	asm volatile("mov %0, %%cr4":: "b"(temp));
*/
}


