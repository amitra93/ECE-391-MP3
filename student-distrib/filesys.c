#include "lib.h"
#include "sched.h"
#include "types.h"
#include "filesys.h"
#include "terminal.h"
#include "rtc.h"

#define ELF_MAGIC_SIZE 4 //Bytes
#define ELF_MAGIC 0x464c457f //Elf Magic number

fops_t rtc_fops = {
	.open = rtc_open,
	.close = rtc_close,
	.write = rtc_write,
	.read = rtc_read
};

fops_t dir_fops = {
	.open = directory_open,
	.close = directory_close,
	.write = directory_write,
	.read=  directory_read
};

fops_t file_fops = {
	.open = file_open,
	.close = file_close,
	.write = file_write,
	.read = file_read
};

fops_t term_fops = {
	.open = terminal_open,
	.close = terminal_close,
	.write = terminal_write,
	.read = terminal_read
};

/* get_inode(uint32_t inode)
 *   DESCRIPTION: Gets a specific inode at location 
 *   INPUTS: inode=location in inode
 *   OUTPUTS: inode pointer
 *   SIDE EFFECTS: none
 */
static inode_t * get_inode(uint32_t inode)
{
	if (inode >= file_sys->num_inodes)
		return NULL;
	return  (inode_t*)(((uint8_t*)file_sys + FILESYS_SIZE) + (inode * sizeof(inode_t)));
}

/* get_datablocks()
 *   DESCRIPTION: Gets the datablock array
 *   INPUTS: none
 *   OUTPUTS: return the datablock array
 *   SIDE EFFECTS: none
 */
static dblock_t * get_datablocks()
{
	return  (dblock_t*)((uint8_t*)get_inode(0) + (file_sys->num_inodes * sizeof(inode_t)));
}

/* init_file_system(uint32_t * start_addr)
 *   DESCRIPTION: Intializes the file system
 *   INPUTS: start_addr=starting address for file system
 *   OUTPUTS: returns 0 on success, otherwise -1
 *   SIDE EFFECTS: Sets the file_system pointer to the starting address
 */
int32_t init_file_system(uint32_t * start_addr)
{
	if (start_addr == NULL)
		return -1;
	file_sys = (file_sys_t * ) start_addr;
	return 0;
}

/* write_file(uint32_t offset, uint8_t* buf, uint32_t length)
 *   DESCRIPTION: Write stub
 *   INPUTS: offset=where to write, buf=where to write it, length=how much to write
 *   OUTPUTS: -1, using a read-file system
 *   SIDE EFFECTS: none
 */
int32_t write_data(uint32_t offset, const uint8_t* buf, uint32_t length)
{
	return -1;
}

int32_t directory_open(const uint8_t* filename) { 
	int i=0;
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	while((curTask->files[i].flags & 0x1) && i<8){
		if(i==7)//at maximum number of files
			return -1;
		i++;
	}
	curTask->files[i].flags = (uint32_t) (1 | (1<<1));
	curTask->files[i].inode = NULL;//ignore bc of directory
	curTask->files[i].offset =1;
	return i; 
}

int32_t directory_close(int32_t fd) { return 0; }
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes) { 
	if(fd<0 || fd>7)
		return -1;
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	if(curTask->files[fd].offset>file_sys->num_inodes)
		return 0;
	uint8_t * targetbuf = (uint8_t *) buf;
	uint8_t * tempbuf = file_sys->dentries[curTask->files[fd].offset].file_name;
	int i=0;
	while(tempbuf[i]!='\0' && i<32){
		targetbuf[i]=tempbuf[i];
		i++;
	}
	curTask->files[fd].offset++;
	return i; 
}
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes) { 
	return -1; 
}


int32_t file_open(const uint8_t* filename) { 

	int i=0;
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	while((curTask->files[i].flags & 0x1) && i<8){
		if(i==7)//at maximum number of files
			return -1;
		i++;
	}
	dentry_t dentry;
	if (read_dentry_by_name (filename, &dentry)<0)
		return -1;//failure to find file!
	curTask->files[i].flags = (uint32_t) (1 | (dentry.file_type<<1));//sets to in use
	curTask->files[i].inode = dentry.inode_num;
	curTask->files[i].offset =0;//init should have set this to 0, but just to be sure
	return i; 
}
int32_t file_close(int32_t fd) 
{ 
	if(fd<2 || fd>6)
		return -1;
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	//if(!(curTask->files[fd].flags & 0x1) )//not open!!
	//	return -1;
	curTask->files[fd].flags=0;
	curTask->files[fd].inode=0;
	curTask->files[fd].offset=0;
	return 0; 

}
int32_t file_read(int32_t fd, void* buf, int32_t nbytes) { 
	if(fd<0 || fd>7)//TODO look to see if should be bw 2 and 7
		return -1;
	task_t * curTask = get_cur_task();
	if(curTask==NULL)
		return -1;
	uint8_t * tempbuf = (uint8_t *) buf;
	inode_t* tempinode = get_inode(curTask->files[fd].inode);
	if (curTask->files[fd].offset >= tempinode->len){
		return 0;
	}
	int32_t returnVal = read_data (curTask->files[fd].inode, curTask->files[fd].offset, tempbuf, nbytes > tempinode->len ? tempinode->len : nbytes);
	if(returnVal!=-1)
		curTask->files[fd].offset+=nbytes;

	return returnVal;

}
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes) { return -1; }

/* read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
 *   DESCRIPTION: Searches for and gets a dentry by its name
 *   INPUTS: fname=file name to search
 *   OUTPUTS: dentry=dentry address to put, returns 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
	uint32_t i,j;
	int32_t equal, found = -1;
	
	if(fname[0]=='.'){
		dentry->file_type = file_sys->dentries[0].file_type;
		dentry->inode_num = file_sys->dentries[0].inode_num;
		dentry->file_name[0] = '.';
		return 0;
	}
	//Search through directory to find file with name
	for (i = 0; i < file_sys->num_dentries; i ++)
	{
		//Check if both the filename are the same
		equal = 1;
		for (j = 0; j < FILENAME_LEN; j ++)
		{
			//If a character is not equal, break the loop
			if (file_sys->dentries[i].file_name[j] != fname[j])
			{
				equal = 0;
				break;
			}
		}
		
		//If it is equal, then we have found and we can break the loop
		if (equal)
		{
			found = i;
			break;
		}
	}
	
	//If it wasn't found then return failure
	if (found == -1)
		return -1;
		
	//Copy over the filename
	for (i = 0; i < FILENAME_LEN; i ++)
		dentry->file_name[i] = file_sys->dentries[found].file_name[i];
	
	//Copy over the other necessary information
	dentry->file_type = file_sys->dentries[found].file_type;
	dentry->inode_num = file_sys->dentries[found].inode_num;
	return 0;
}

/* read_dentry_by_index (uint32_t index, dentry_t* dentry)
 *   DESCRIPTION: Gets a dentry by its index
 *   INPUTS: index=which dentry
 *   OUTPUTS: dentry=address to dentry, 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
	uint32_t i;
	
	//Return failure if index is out of bounds
	if (index >= file_sys->num_dentries)
		return -1;
	
	//Copy over the filename
	for (i = 0; i < FILENAME_LEN; i ++)
		dentry->file_name[i] = file_sys->dentries[index].file_name[i];
		
	//Copy over the other necessary information
	dentry->file_type = file_sys->dentries[index].file_type;
	dentry->inode_num = file_sys->dentries[index].inode_num;
	return 0;
}

/* read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
 *   DESCRIPTION: Reads data from file into buf (caller must check filetype)
 *   INPUTS: inode=which inode to read, offset=offset into file, length=how much to write
 *   OUTPUTS: buf=reads from file, returns number of bytes read on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	inode_t * node;
	dblock_t * dblocks;
	dblock_t * dblock;
	uint32_t dblock_page;
	uint32_t i, block_length;
		
	//Return failure if index is out of bounds
	if (inode >= file_sys->num_inodes)
		return -1;
	node = get_inode(inode);
	
	//Determine the lnumber of datablocks for this node
	block_length = (node->len / DATABLOCK_SIZE) + 1;
	
	//The starting data block page
	dblock_page = offset / DATABLOCK_SIZE;
	
	//Check bounds
	if (dblock_page >= block_length)
		return -1;
	
	//The starting offset into the dblock
	offset %= DATABLOCK_SIZE;
	
	//Set up the data block arrayand
	dblocks = get_datablocks();
	
	//Get the first data block
	dblock = &dblocks[node->dblocks[dblock_page]];
	
	//Check bounds
	if (node->dblocks[dblock_page] >= file_sys->num_blocks)
		return -1;
	
	//Iterate through and copy memory
	for (i = 0; i < length; i ++)
	{
		//Copy byte
		buf[i] = dblock->data[i + offset];
		
		//If the next byte is in the next page, get the beginning of next page
		if ( (i + 1 + offset) % DATABLOCK_SIZE == 0)
		{
			//Get the next page
			dblock_page++;
			
			//Double check for end of file
			if (dblock_page == block_length)
				return i;
			
			//Double check bounds
			else if (dblock_page > block_length)
				return -1;
			if (node->dblocks[dblock_page] >= file_sys->num_blocks)
				return -1;
			dblock = &dblocks[node->dblocks[dblock_page]];
			
			//Reset to beginning of page
			offset = 0;
		}
	}
	
	//Return number of bytes read
	return i;
}

/* load_program (const uint8_t* fname, uint8_t * pgrm_addr)
 *   DESCRIPTION: Loads a program by file name into prgm_addr
 *   INPUTS: fname=file to read and copy, pgrm_addr=the starting address of file copy
 *   OUTPUTS: returns number of bytes copied, or -1 for failure
 *   SIDE EFFECTS: copies a program into physical memory
 */
int32_t load_program(const uint8_t* fname, uint8_t * pgrm_addr)
{
	uint8_t header[ELF_MAGIC_SIZE];
	dentry_t dentry;
	
	//Check for valid pointers
	if (fname == NULL || pgrm_addr == NULL)
		return -1;
		
	//Get the directory entry for the program to copy	
	if (read_dentry_by_name(fname, &dentry) == -1)
		return -1;

	//Make sure it's the correct file type
	if (dentry.file_type != 2)
		return -1;
	
	//Get the header, return failure if it can't get header
	if (read_data(dentry.inode_num, 0, header, ELF_MAGIC_SIZE) != ELF_MAGIC_SIZE)
		return -1;
	
	//Make sure we have an ELF
	if (*((uint32_t*)(header)) != ELF_MAGIC)
		return -1;
	
	//Copy the data into memory (To go to the end of the file, set -1 which
	//will translate into the maximum value for uint32_t
	read_data(dentry.inode_num, 0, pgrm_addr, -1);
	
	//Return the entry point
	return *((uint32_t*)(pgrm_addr + 24));
}

void test_loader()
{


}

void test_file_system()
{
	int i;
	dentry_t dentry;
	uint8_t file_names [16][FILENAME_LEN] = {
		{"."},
		{"frame1.txt"},
		{"verylargetxtwithverylongname.tx\0"},
		{"ls"},
		{"grep"},
		{"hello"},
		{"rtc"},
		{"testprint"},
		{"sigtest"},
		{"shell"},
		{"syserr"},
		{"fish"},
		{"cat"},
		{"frame0.txt"},
		{"pingpong"},
		{"counter"},
	};
	
	for (i = 0; i < 16; i ++)
	{
		int j = 0;
		uint8_t buf[4000];
		clear();
		set_cursor_pos(0,1);
		printf("Searching for %s\n", file_names[i]);
		read_dentry_by_name(file_names[i], &dentry);
		printf("File name is: %s\n", dentry.file_name);
		printf("File type is: %d\n", dentry.file_type);
		printf("Inode num is: %d\n", dentry.inode_num);
		printf("\n");
		printf("Get by index: %d\n", i);
		read_dentry_by_index(i, &dentry);
		printf("File name is: %s\n", dentry.file_name);
		printf("File type is: %d\n", dentry.file_type);
		printf("Inode num is: %d\n", dentry.inode_num);
		printf("\n");
		
		if (dentry.file_type == 2)
		{
			printf("Reading first 160 Bytes...\n");
			read_data(dentry.inode_num, 0, buf, 4000);
			terminal_write(0, buf, 4000);
			//printf("%s\n", buf);
			
			read_data(dentry.inode_num, 40, buf, 40);
			//printf("%s\n", buf);
			
			read_data(dentry.inode_num, 80, buf, 40);
		//	printf("%s\n", buf);
			
			read_data(dentry.inode_num, 120, buf, 40);
			//printf("%s\n", buf);
		}
		for(j = 0; j < 1000000000; j ++);
	}
}

