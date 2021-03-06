#include "types.h"
#include "sched.h"
#include "lib.h"
#include "filesys.h"
#include "terminal.h"
#include "paging.h"

/*
 *int32_t return_from_halt(uint8_t status, uint32_t ebp, uint32_t esp, uint32_t eip);
 *DESCRIPTION: This function returns to the task's return addresses to the parent task's execute to halt
 *
 *INPUTS: status, ebp, esp, eip
 *OUTPUTS: none
 *SIDE EFFECTS: halts current task
 */
extern void return_from_halt(uint8_t status, uint32_t ebp, uint32_t esp, uint32_t eip);

/*
 *void restart_shell(uint32_t eip, uint16_t cs, uint32_t eflags, uint32_t esp, uint16_t ss);
 *DESCRIPTION: if a shell was halted, then we don't want to restart the whole task, so we only restart to the initial state
 *
 *INPUTS: eip, cs, eflags, esp, ss (inputs to a context switch)
 *OUTPUTS: none
 *SIDE EFFECTS: restarts the current shell
 */
extern void restart_shell(uint32_t eip, uint16_t cs, uint32_t eflags, uint32_t esp, uint16_t ss);

/*
 *int32_t is_user_ptr(const void * ptr) 
 *DESCRIPTION: this function figures out if it is a user pointer or not
 *
 *INPUTS: ptr
 *OUTPUTS: returns -1 if it is not a user pointer, 0 if it is a user pointer
 *SIDE EFFECTS: none
 */
int32_t is_user_ptr(const void * ptr)
{
	uint32_t addr = (uint32_t)ptr;
	uint32_t task_addr = get_task_addr(get_cur_task()->pid);
	if (addr < task_addr || addr >= task_addr + SIZE_4MB_PAGE)
		return -1;
	else
		return 0;
}
 /*
 *int32_t do_halt (uint8_t status) 
 *DESCRIPTION: terminates a process, returning the specified value to its parent process
 *
 *INPUTS: status
 *OUTPUTS: returns 0
 *SIDE EFFECTS: halts and stops a process
 */
int32_t do_halt (uint8_t status) 
{ 
	// Go to the next line in the terminal
	int x, y;
	get_cursor_pos(&x, &y);
	if (x > 0){
		char str = '\n';
		terminal_write(1, &str, 1);
	}
	
	// If the task to halt is a terminal, then restart the shell
	task_t * cur_task = get_cur_task();
	if (cur_task->pid == 1 || cur_task->pid == 2 || cur_task->pid == 3)
	{
		cur_task->tss.eip = cur_task->ret_eip;
		cur_task->tss.eflags = cur_task->ret_eflags;
		cur_task->tss.esp = cur_task->ret_esp;
		load_tss(cur_task);
		restart_shell(cur_task->ret_eip, USER_CS, cur_task->ret_eflags, cur_task->ret_esp, USER_DS);
	}
	
	//Otherwise halt the current task
	return_from_halt(status, cur_task->ret_ebp, cur_task->ret_esp, cur_task->ret_eip);
	return 0;
}

 /*
 *int32_t do_execute (const uint8_t* command) 
 *DESCRIPTION: attempts to load an execute a new program, handing off the processor
               to the new program until it terminates.
 *
 *INPUTS: command
 *OUTPUTS: returns -1 if command cannot be executed
 *SIDE EFFECTS: processor gets handed off to the new program until termination 
 */
int32_t do_execute (const uint8_t* command) 
{ 
	//argsBuffer holds the newly formatted args
	uint8_t argsBuffer [128];
	if(command==NULL)
		return -1;
	uint8_t commandBufferIndex=0,argsBufferIndex=0, pgmNameIndex=0, gotProgamName=0;
	uint8_t programName[32];
	uint32_t i;
	
	//This loop parses the arguments from the console
	while (command[commandBufferIndex]!='\0' && command[commandBufferIndex]!='\n')
	{
		i = commandBufferIndex;
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
				programName[pgmNameIndex++]='\0';
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
	
	//Fill the rest of the program name with nothing
	for (i = pgmNameIndex; i < 32; i ++)
		programName[i] = '\0';
	
	//Create and execute the task!!
	int32_t pid = create_task(programName, argsBuffer);
	if (pid < 0)
		return -1;
	return execute_task((uint32_t)pid);
}

 /*
 *int32_t do_read (int32_t fd, void* buf, int32_t nbytes) 
 *DESCRIPTION: reads data from the keyboard, a file, device, or directory
 *
 *INPUTS: fd, buf, nbytes
 *OUTPUTS: returns returns number of bytes read, or 0 which indicates EOF has been reached
 *SIDE EFFECTS: For keyboard: return data from one line that has been terminated
                by pressing Enter, or as much as fits in the buffer
                For file: data should be read to EOF or end of buffer provided
 */
int32_t do_read (int32_t fd, void* buf, int32_t nbytes) 
{
	//Double check that the pointer is from user space
	if (is_user_ptr(buf))
		return fd < 0 ? -1 : fd > 7 ? -1 : (get_cur_task()->files[fd].flags)&FILE_OPEN_BIT ? get_cur_task()->files[fd].fops->read(fd, buf, nbytes) : -1;
	return -1;
}

/*
 *int32_t do_write (int32_t fd, const void* buf, int32_t nbytes) 
 *DESCRIPTION: writes data to terminal or to a device
 *
 *INPUTS: fd, buf, nbytes
 *OUTPUTS: returns -1 for failure
 *SIDE EFFECTS: terminal: all data should be displayed to screen immediately, 
                for RTC, only accepts 4 byte integer specifying interrupt rate
 */
int32_t do_write (int32_t fd, const void* buf, int32_t nbytes) 
{
	//Double check that the pointer is from user space
	if (is_user_ptr(buf))
		return fd < 0 ? -1 : fd > 7 ? -1 : (get_cur_task()->files[fd].flags)&FILE_OPEN_BIT ? get_cur_task()->files[fd].fops->write(fd, buf, nbytes) : -1;
	return -1;
}

 /*
 *int32_t do_vidmap (uint8_t** screen_start)
 *DESCRIPTION: maps text-mode video memory into user space at a pre-set virtual address.
 *
 *INPUTS: screen_start
 *OUTPUTS: if invalid location, returns -1.
 *SIDE EFFECTS: none
 */
int32_t do_vidmap (uint8_t** screen_start) 
{
	//Double check that the pointer is from user space
	if (!is_user_ptr((uint8_t*)screen_start))
		return -1;
	*screen_start = (uint8_t*)(VIRTUAL_VID_MEM + VIDEO);
	return 0;
}

 /*
 *int32_t do_open (const uint8_t* filename)
 *DESCRIPTION: provides access to the file system, finds directory
               entry corresponding to the filename.
 *
 *INPUTS: filename
 *OUTPUTS: returns -1 if no file name exists or descriptors
 *SIDE EFFECTS: allocates an unused file descriptor, and sets up
                any data necessary to handle the given type of file
 */
int32_t do_open (const uint8_t* filename) { 
	//find the directory entry corresponding to the named file
	dentry_t dentry;
	uint8_t fname [32] = {'\0'};
	uint32_t i = 0;
	
	//Copy over the filename into a local buffer
	while (*filename != '\0')
	{
		fname[i] = *filename;
		filename ++;
		i ++;
	}
	
	//Find the file
	if (read_dentry_by_name (fname, &dentry)<0)
		return -1;//failure to find file!

	//Try to open the file
	i=2;
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	//Find a file descriptor to use
	while((curTask->files[i].flags & FILE_OPEN_BIT) && i<8){
		if(i==7)//at maximum number of files
			return -1;
		i++;
	}
	
	//Different file types have different fops
	switch (dentry.file_type)
	{
		case 0:
			curTask->files[i].flags = (1<<dentry.file_type) | FILE_OPEN_BIT;
			curTask->files[i].fops = &rtc_fops;
			break;	
		case 1:
			curTask->files[i].flags = (1<<dentry.file_type) | FILE_OPEN_BIT;
			curTask->files[i].fops = &dir_fops;
			break;
		case 2:
			curTask->files[i].flags = (1<<dentry.file_type) | FILE_OPEN_BIT;
			curTask->files[i].fops = &file_fops;
			break;
		default:
			
			return -1;
	}
	curTask->files[i].dentry = dentry;
	curTask->files[i].inode = get_inode(dentry.inode_num);
	curTask->files[i].offset = 0;
	
	//If the fops open fails...
	if(curTask->files[i].fops->open(filename) < 0)
		return -1;
		
	//Success and return the FD
	return i;
}

 /*
 *int32_t do_close (int32_t fd)
 *DESCRIPTION: closes the specified file descriptor and makes it available
               for return from later calls to open
 *
 *INPUTS: fd
 *OUTPUTS: successful close returns 0, else returns a -1
 *SIDE EFFECTS: makes fd available for return from later calls to open
 */
int32_t do_close (int32_t fd) { 
	task_t * curTask = get_cur_task();
	
	//Check the bounds of the file descriptor
	if(curTask==NULL || fd < 2 || fd>7)
		return -1;
	if ((curTask->files[fd].flags & FILE_OPEN_BIT) != 1)
		return -1;

	curTask->files[fd].fops->close(fd);
	curTask->files[fd].flags =0;
	curTask->files[fd].inode = NULL;
	curTask->files[fd].offset =0;
	curTask->files[fd].fops = NULL;

	return 0; 
}

 /*
 *int32_t do_getargs (uint8_t* buf, int32_t nbytes)
 *DESCRIPTION: reads program's command line arguments into a user-level buffer.
 *
 *INPUTS: buf, nbytes
 *OUTPUTS: returns -1 if args and terminal NULL do not fit in buffer.
 *SIDE EFFECTS: args are copied into user space
 */
int32_t do_getargs (uint8_t* buf, int32_t nbytes) {
	
	//Check bounds of user pointer to copy
	if (is_user_ptr(buf))
	{
		task_t * curTask = get_cur_task();
		if(curTask==NULL || buf==NULL)
			return -1;
		int i, size = 0;

		while (curTask->args[size] != '\0' && size<nbytes){
			size++;
		}

		if (size == nbytes - 1){
			return -1;
		}

		//Copy over the current task's arguments
		for(i=0; i<=size; i++){
			 buf[i]=curTask->args[i];
		}
		return 0; 
	}
	return -1;
}

 /*
 *int32_t do_set_handler (int32_t signum, void* handler_address)
 *DESCRIPTION: changes the default action taken when a signal is received
 *
 *INPUTS: signum, handler_address
 *OUTPUTS: returns -1 for failure
 *SIDE EFFECTS: changes the default action for a received signal
 */
int32_t do_set_handler (int32_t signum, void* handler_address) { return -1; }

 /*
 *int32_t do_sigreturn (void)
 *DESCRIPTION: copies hardware context that was on user level stack back to 
			   the processor
 *
 *INPUTS: none
 *OUTPUTS: returns -1 for failure
 *SIDE EFFECTS: none
 */
int32_t do_sigreturn (void) { return -1; }
