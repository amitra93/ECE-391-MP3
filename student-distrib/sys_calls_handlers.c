#include "types.h"

#define GET_ARGS(arg0, arg1, arg2) 											\
	do { 									 								\
		asm volatile("														\
		movl %%ebx, %0 \n movl %%ecx, %1 \n movl %%edx, %2"					\
		:"=r"(arg0), "=r"(arg1), "=r"(arg2), "=r"(arg3)::"%esi","%edi");	\
	}while(0)

uint8_t argsBuffer [128];	
	
int32_t do_halt (uint8_t status) { return 0; }
int32_t do_execute (const uint8_t* command) 
{ 
	//sudo get args (get from mitra and store in variable)
	//argsBuffer holds the newly formatted args, terminalBuffer is what text terminal has at execution 
	//uint8_t *terminalBuffer = magicalMitraFunction();
	char trash1,trash2;//just to be safe so we don't break anything
	GET_ARGS(command, trash1, trash2);
	char terminalBufferIndex=0,argsBufferIndex=0, pgmNameIndex=0, gotPgmName=0;
	uint8_t programName[64];
	while (buffer[terminalBufferIndex]!='\0')
	{
		int i = terminalBufferIndex;
		while(buffer[i]==' ')//if multiple spaces, go to the last space
			i++; 
		if(i!=terminalBufferIndex) //we found at least one space
		{	
			if(gotProgamName)//remove spaces from args and copy to 
			{
				argsBuffer[argsBufferIndex]=' ';
				argsBufferIndex++;
			}
			else
			{
				gotProgamName=1;
				programName[pgmNameIndex]='\0'
			}
			terminalBufferIndex =i;//skip to current index accounting for all spaces
		}
		//char copy and increment for the next iteration
		if(gotProgamName)//putting args into argsbuffer
			argsBuffer[argsBufferIndex++]=terminalBuffer[terminalBufferIndex++]; 	
		else//put into programName buffer
			programName[pgmNameIndex++]=terminalBuffer[terminalBufferIndex++]; 
	}
	argsBuffer[argsBufferIndex] ='\0';
	
	//Create page directory...or is it just a page?
	//Load program into memory
		//load_program(&programName, uint8_t * pgrm_addr)

	/*	Create new task struct
			1) Get PID
			2) FD Array: Sets the Stdin, Stdout
			3) Sets children/parent/siblings
	
		Context Switch
			1) Switch TSS 
				a) Switching the segments
				b) Switching CR3
			2) Set up Stack
			3) IRET*/
	
	return 0; 
}
int32_t do_read (int32_t _fd, void* buf, int32_t _nbytes) { return 0; }
int32_t do_write (int32_t _fd, const void* buf, int32_t _nbytes) { return 0; }
int32_t do_open (const uint8_t* filename) { return 0; }
int32_t do_close (int32_t _fd) { return 0; }
int32_t do_getargs (uint8_t* buf, int32_t _nbytes) {


return 0; 
}
int32_t do_vidmap (uint8_t** screen_start) { return 0; }
int32_t do_set_handler (int32_t _signum, void* handler_address) { return 0; }
int32_t do_sigreturn (void) { return 0; }
