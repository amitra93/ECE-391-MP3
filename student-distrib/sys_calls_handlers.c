#include "types.h"
#include "sched.h"
#include "lib.h"

#define setup_return_stack(task) 							\
	do {													\
		asm volatile("										\
			pushl %0 \n										\
			pushl %1 \n										\
			pushl %2 \n										\
			pushl %3 \n										\
			pushl %4 \n										\
			movw $0x18, %%ax \n								\
			movw %%ax, %%ds	\n								\
			"::"a"((task)->tss.ss),							\
			"b"((task)->tss.esp),							\
			"c"((task)->tss.eflags),						\
			"d"((task)->tss.cs),							\
			"S"((task)->tss.eip));							\
			asm volatile("									\
				movl %0, %%ebp":: "r"((task)->tss.ebp));	\
	}while(0)

#define iret()					\
	do { 						\
		asm volatile("iret");	\
	}while(0)

	
#define return_from_syscall()	\
		do { 					\
			asm volatile("		\
				addl $20, %%esp	\
				popl %%edi		\
				popl %%esi		\
				popl %%ebx		\
				iret ");		\
	}while(0)
	
#define return_from_halt()								\
		do { 											\
			asm volatile("								\
				addl $44, %esp");						\
				goto *((get_cur_task())->halt_addr);	\
	}while(0)					

#define GET_ARGS(arg0, arg1, arg2) 											\
	do { 									 								\
		asm volatile("														\
		movl %%ebx, %0 \n movl %%ecx, %1 \n movl %%edx, %2"					\
		:"=r"(arg0), "=r"(arg1), "=r"(arg2)::"%esi","%edi");	\
	}while(0)

uint8_t argsBuffer [128];	
	
int32_t do_halt (uint8_t status) 
{ 
	task_t * parent_task = get_cur_task()->parent_task;
	setup_return_stack(parent_task);
	iret();
}

int32_t do_execute (const uint8_t* command) 
{ 
	#if 0
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
	#endif
	
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
	{
		int32_t pid;
		uint8_t fname [32] = "testprint";
		uint8_t args[128] = "nothing nothing nothing";
		//print_error("Test", 0, 0, 1);
		pid = create_task(fname, args);
		if (pid >= 0)
			switch_task(pid);
	}
	//while(1);
	
	return 0; 
}
int32_t do_read (int32_t _fd, void* buf, int32_t _nbytes) { return 0; }
int32_t do_write (int32_t _fd, const void* buf, int32_t _nbytes) { return 0; }
int32_t do_open (const uint8_t* filename) { return 0; }
int32_t do_close (int32_t _fd) { return 0; }
int32_t do_getargs (uint8_t* buf, int32_t _nbytes) {


	return 0; 
}
int32_t do_vidmap (uint8_t** screen_start) 
{
	return 0;
}
int32_t do_set_handler (int32_t _signum, void* handler_address) { return 0; }
int32_t do_sigreturn (void) { return 0; }
