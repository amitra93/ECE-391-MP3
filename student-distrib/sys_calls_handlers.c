#include "types.h"
#include "sched.h"
#include "lib.h"
#include "filesys.h"
#include "terminal.h"
#include "paging.h"

extern void return_from_halt(uint8_t status, uint32_t ebp, uint32_t esp, uint32_t eip);
extern void restart_shell(uint32_t eip, uint16_t cs, uint32_t eflags, uint32_t esp, uint16_t ss);

int32_t is_user_ptr(const void * ptr)
{
	uint32_t addr = (uint32_t)ptr;
	uint32_t task_addr = get_task_addr(get_cur_task()->pid);
	if (addr < task_addr || addr >= task_addr + SIZE_4MB_PAGE)
		return -1;
	else
		return 0;
}

int32_t do_halt (uint8_t status) 
{ 
	int x, y;
	get_cursor_pos(&x, &y);
	if (x > 0){
		char str = '\n';
		terminal_write(1, &str, 1);
	}
	task_t * cur_task = get_cur_task();
	if (cur_task->pid == 1 || cur_task->pid == 2 || cur_task->pid == 3)
	{
		cur_task->tss.eip = cur_task->ret_eip;
		cur_task->tss.eflags = cur_task->ret_eflags;
		cur_task->tss.esp = cur_task->ret_esp;
		load_tss(cur_task);
		restart_shell(cur_task->ret_eip, USER_CS, cur_task->ret_eflags, cur_task->ret_esp, USER_DS);
	}
	return_from_halt(status, cur_task->ret_ebp, cur_task->ret_esp, cur_task->ret_eip);
	return 0;
}

int32_t do_execute (const uint8_t* command) 
{ 
	//argsBuffer holds the newly formatted args
	uint8_t argsBuffer [128];
	if(command==NULL)
		return -1;
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
	
	int32_t pid = create_task(programName, argsBuffer);
	if (pid < 0)
		return -1;
	return execute_task((uint32_t)pid);
}
int32_t do_read (int32_t fd, void* buf, int32_t nbytes) 
{
	if (is_user_ptr(buf))
		return fd < 0 ? -1 : fd > 7 ? -1 : (get_cur_task()->files[fd].flags)&0x1 ? get_cur_task()->files[fd].fops->read(fd, buf, nbytes) : -1;
	return -1;
}
int32_t do_write (int32_t fd, const void* buf, int32_t nbytes) 
{
	if (is_user_ptr(buf))
		return fd < 0 ? -1 : fd > 7 ? -1 : (get_cur_task()->files[fd].flags)&0x1 ? get_cur_task()->files[fd].fops->write(fd, buf, nbytes) : -1;
	return -1;
}
int32_t do_vidmap (uint8_t** screen_start) 
{
	if ((uint8_t*)screen_start < (uint8_t*)0x8000000 || (uint8_t*)screen_start >= (uint8_t*)0x8400000)
		return -1;
	*screen_start = (uint8_t*)(VIRTUAL_VID_MEM + VIDEO);
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
	if(curTask==NULL || fd < 2 || fd>7)
		return -1;
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

		for(i=0; i<=size; i++){
			 buf[i]=curTask->args[i];
		}
		return 0; 
	}
	return -1;
}

int32_t do_set_handler (int32_t signum, void* handler_address) { return -1; }

int32_t do_sigreturn (void) { return -1; }
