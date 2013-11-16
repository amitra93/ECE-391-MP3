#include "types.h"
#include "sched.h"
#include "lib.h"

//different than the get_args system call...literally gets the arguments for each 
//system call
#define GET_ARGS(arg0, arg1, arg2) 											\
	do { 									 								\
		asm volatile("														\
		movl %%ebx, %0	\n													\
		movl %%ecx, %1	\n													\
		movl %%edx, %2"														\	
		:"=r"(arg0), "=r"(arg1), "=r"(arg2)::"%esi","%edi");				\
	}while(0)


	
int32_t do_halt (uint8_t status) { return 0; }
int32_t do_execute () 
{ 
	
	//argsBuffer holds the newly formatted args
	uint8_t argsBuffer [128];
	uint32_t cmd; 
	uint8_t * command;
	uint32_t trash1,trash2;//just to be safe so we don't break anything
	//asm volatile("movl %%ebx, %0 \n movl %%ecx, %1 \n movl %%edx, %2":"=r"(command), "=r"(trash1), "=r"(trash2));			
	GET_ARGS(cmd,trash1, trash2);
	command = (uint8_t*)cmd;
	uint8_t commandBufferIndex=0,argsBufferIndex=0, pgmNameIndex=0, gotProgamName=0;
	uint8_t programName[64];
	while (command[commandBufferIndex]!='\0')
	{
		int i = commandBufferIndex;
		while(command[i]==' ')//if multiple spaces, go to the last space
			i++; 
		if(i!=commandBufferIndex) //we found at least one space
		{	
			if(gotProgamName)//remove spaces from args and copy to 
			{
				argsBuffer[argsBufferIndex]=' ';
				argsBufferIndex++;
			}
			else
			{
				gotProgamName=1;
				programName[pgmNameIndex]='\0';
			}
			commandBufferIndex =i;//skip to current index accounting for all spaces
		}
		//char copy and increment for the next iteration
		if(gotProgamName)//putting args into argsbuffer
			argsBuffer[argsBufferIndex++]=command[commandBufferIndex++]; 	
		else//put into programName buffer
			programName[pgmNameIndex++]=command[commandBufferIndex++]; 
	}
	argsBuffer[argsBufferIndex] ='\0';
	programName[pgmNameIndex] ='\0';
	
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
	//print_error("Test", 0, 0, 1);
	int32_t pid = create_task("testprint"/*programName*/, argsBuffer);
	if (pid >= 0)
		switch_task((uint32_t)pid);
	
	return 0; 
}
int32_t do_read (int32_t _fd, void* buf, int32_t _nbytes) { return 0; }
int32_t do_write (int32_t _fd, const void* buf, int32_t _nbytes) { return 0; }
int32_t do_open (const uint8_t* filename) { 
	//find the directory entry corresponding to the named file
	dentry_t dentry;
	if (read_dentry_by_name (filename, &dentry)<0)
		return -1;//failure to find file!
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	int i=0;
	while(curTask->files[i].flags && i<8){
		if(i==7)//at maximum number of files
			return -1;
	}

	curTask->files[i].flags |= 1;//in case other bits are used for other things later...
	curTask->files[i].inode = &dentry;
	curTask->files[i].offset =0;//init should have set this to 0, but just to be sure
	//TODO somehow set fops commands based on type...
	//files[i].fops = //set based on type
	//TODO call open() somehow

	return i; 
}
int32_t do_close (int32_t _fd) { return 0; }
int32_t do_getargs (uint8_t* buf, int32_t _nbytes) {

	return 0; 
}
int32_t do_vidmap (uint8_t** screen_start) { return 0; }
int32_t do_set_handler (int32_t _signum, void* handler_address) { return 0; }
int32_t do_sigreturn (void) { return 0; }
