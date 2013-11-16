#include "types.h"
#include "sched.h"
#include "lib.h"

<<<<<<< HEAD
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

=======
//different than the get_args system call...literally gets the arguments for each 
//system call
>>>>>>> 0354ecea9f56ca107f31f74c8d79d7493621e5ce
#define GET_ARGS(arg0, arg1, arg2) 											\
	do { 									 								\
		asm volatile("														\
		movl %%ebx, %0	\n													\
		movl %%ecx, %1	\n													\
		movl %%edx, %2 \n"													\
		:"=r"(arg0), "=r"(arg1), "=r"(arg2)::"%esi","%edi");				\
	}while(0)


	
<<<<<<< HEAD
int32_t do_halt (uint8_t status) 
{ 
	task_t * parent_task = get_cur_task()->parent_task;
	setup_return_stack(parent_task);
	iret();
}

int32_t do_execute (const uint8_t* command) 
=======
int32_t do_halt (uint8_t status) { return 0; }
int32_t do_execute () 
>>>>>>> 0354ecea9f56ca107f31f74c8d79d7493621e5ce
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
<<<<<<< HEAD
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
=======
	//print_error("Test", 0, 0, 1);
	int32_t pid = create_task(programName, argsBuffer);
	if (pid >= 0)
		switch_task((uint32_t)pid);
>>>>>>> 0354ecea9f56ca107f31f74c8d79d7493621e5ce
	
	return 0; 
}
int32_t do_read (int32_t _fd, void* buf, int32_t _nbytes) { return 0; }
int32_t do_write (int32_t _fd, const void* buf, int32_t _nbytes) {
	//TODO HOW THE FUCK DO YOU MAKE THIS ONE LINE
	task_t * curTask = get_cur_task();
	uint8_t fileType = (curTask->files[_fd].flags>>1)& 0x3; //check the second and third bits of flags for file type
	switch(fileType){
		case 0://file is an RTC
			
			break;
		case 1://file is a directory
			break;
		case 2://regular file
			return -1;//read only file system!!! 
			break;
		default://invalid file type (checks from before should have prevented this)
			return -1; 

	}
	return 0; 

<<<<<<< HEAD
	return 0; 
}
int32_t do_vidmap (uint8_t** screen_start) 
{
	return 0;
=======
}
int32_t do_open (const uint8_t* filename) { 
	//find the directory entry corresponding to the named file
	dentry_t dentry;
	if (read_dentry_by_name (filename, &dentry)<0)
		return -1;//failure to find file!
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	int i=0;
	while((curTask->files[i].flags & 0x1) && i<8){
		if(i==7)//at maximum number of files
			return -1;
	}

	curTask->files[i].flags = (1 | (dentry.file_type<<1));//sets to in use
	//TODO figure out correct syntax
	//curTask->files[i].inode = &dentry;
	curTask->files[i].offset =0;//init should have set this to 0, but just to be sure
	//bit 1 -> if is in use
	//bits 3 & 2: (remember to use masking when setting)
	// value 0: regular file
	// value 1: directory
	// value 3: RTC
	//TODO somehow set fops commands based on type...
	//files[i].fops = //set based on type
	//TODO call open() somehow

	return i; 
}
int32_t do_close (int32_t _fd) { 
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	curTask->files[_fd].flags =0;
	curTask->files[_fd].inode = NULL;
	curTask->files[_fd].offset =0;
	//TODO reset fops
	return 0; 
}
int32_t do_getargs (uint8_t* buf, int32_t _nbytes) {

	return 0; 
>>>>>>> 0354ecea9f56ca107f31f74c8d79d7493621e5ce
}
int32_t do_set_handler (int32_t _signum, void* handler_address) { return 0; }
int32_t do_sigreturn (void) { return 0; }
