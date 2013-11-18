#include "types.h"
#include "sched.h"
#include "lib.h"
#include "filesys.h"

extern void return_from_halt(uint8_t status, uint32_t ebp, uint32_t esp, uint32_t eip);

int32_t do_halt (uint32_t status) 
{ 
	task_t * parent_task = get_cur_task()->parent_task;
	return_from_halt((uint8_t)status, parent_task->tss.ebp, parent_task->tss.esp, parent_task->tss.eip);
	return 0;
}

int32_t do_execute (const uint8_t* command) 
{ 
	//argsBuffer holds the newly formatted args
	uint8_t argsBuffer [128];

	uint8_t commandBufferIndex=0,argsBufferIndex=0, pgmNameIndex=0, gotProgamName=0;
	uint8_t programName[32];
	uint32_t i;
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
	for (i = pgmNameIndex; i < 32; i ++)
		programName[i] = 0;
	
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
	int32_t pid = create_task(programName, argsBuffer);
	if (pid >= 0)
		switch_task((uint32_t)pid);
	
	return 0; 
}
int32_t do_read (int32_t fd, void* buf, int32_t nbytes) 
{
	return fd < 0 ? -1 : ((get_cur_task()->files[fd].flags)&0x1 ? get_cur_task()->files[fd].fops->read(fd, buf, nbytes) : -1);
}
int32_t do_write (int32_t fd, const void* buf, int32_t nbytes) 
{
	return fd < 0 ? -1 : ((get_cur_task()->files[fd].flags)&0x1 ? get_cur_task()->files[fd].fops->write(fd, buf, nbytes) : -1);
}
int32_t do_vidmap (uint8_t** screen_start) 
{
	if ((uint8_t*)screen_start < (uint8_t*)0x8000000 || (uint8_t*)screen_start >= (uint8_t*)0x8400000)
		return -1;
	*screen_start = get_cur_task()->video_mem;
	return 0;
}
int32_t do_open (const uint8_t* filename) { 
	//find the directory entry corresponding to the named file
	dentry_t dentry;
	uint8_t fname [32] = {'\0'};
	uint32_t i = 0;
	
	while (*filename != '\0')
	{
		fname[i] = *filename;
		filename ++;
		i ++;
	}
	
	if (read_dentry_by_name (fname, &dentry)<0)
		return -1;//failure to find file!

	i=0;
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	while((curTask->files[i].flags & 0x1) && i<8){
		if(i==7)//at maximum number of files
			return -1;
		i++;
	}
	switch (dentry.file_type)
	{
		case 0:
			curTask->files[i].flags = (1<<dentry.file_type) | 1;
			curTask->files[i].fops = &rtc_fops;
			break;	
		case 1:
			curTask->files[i].flags = (1<<dentry.file_type) | 1;
			curTask->files[i].fops = &dir_fops;
			break;
		case 2:
			curTask->files[i].flags = (1<<dentry.file_type) | 1;
			curTask->files[i].fops = &file_fops;
			break;
		default:
			return -1;
	}
	curTask->files[i].dentry = dentry;
	curTask->files[i].inode = get_inode(dentry.inode_num);
	curTask->files[i].offset = 0;
	if(curTask->files[i].fops->open(filename) < 0)
		return -1;
	return i;
}
int32_t do_close (int32_t fd) { 
	task_t * curTask = get_cur_task();
	if(curTask==NULL || fd < 2 || fd>6)
		return -1;
		
	/*
	if(curTask->files[fd].fops != NULL)
	{
		return curTask->files[fd].fops->close(fd);
	}
	else{
		curTask->files[fd].flags =0;
		curTask->files[fd].inode = NULL;
		curTask->files[fd].offset =0;
		curTask->files[fd].fops = NULL;
		return 0;
	}*/

	if ((curTask->files[fd].flags & 0x1) != 1)
		return -1;
	curTask->files[fd].fops->close(fd);
	curTask->files[fd].flags =0;
	curTask->files[fd].inode = NULL;
	curTask->files[fd].offset =0;
	curTask->files[fd].fops = NULL;

	return 0; 
}
int32_t do_getargs (uint8_t* buf, int32_t nbytes) {
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

	for(i=0; i<=size; i++){
		 buf[i]=curTask->args[i];
	}
	return 0; 
}

int32_t do_set_handler (int32_t signum, void* handler_address) { return -1; }

int32_t do_sigreturn (void) { return -1; }
