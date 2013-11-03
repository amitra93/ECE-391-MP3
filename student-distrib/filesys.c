#include "lib.h"
#include "filesys.h"

inode_t * get_inode(uint32_t inode)
{
	if (inode >= file_sys->num_inodes)
		return NULL;
	return  (inode_t*)(((uint8_t*)file_sys + FILESYS_SIZE) + (inode * sizeof(inode_t)));
}

dblock_t * get_datablocks()
{
	return  (dblock_t*)((uint8_t*)get_inode(0) + (file_sys->num_inodes * sizeof(inode_t)));
}

int32_t init_file_system(uint32_t * start_addr)
{
	file_sys = (file_sys_t * ) start_addr;
	return 0;
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry)
{
	uint32_t i,j;
	uint32_t equal, found = -1;
	for (i = 0; i < file_sys->num_dentries; i ++)
	{
		equal = 1;
		for (j = 0; j < FILENAME_LEN; j ++)
		{
			if (file_sys->dentries[i].file_name[j] != fname[j])
			{
				equal = 0;
				break;
			}
		}
		if (equal)
		{
			found = i;
			break;
		}
	}
	
	if (found == -1)
		return -1;
	for (i = 0; i < FILENAME_LEN; i ++)
		dentry->file_name[i] = file_sys->dentries[found].file_name[i];
	dentry->file_type = file_sys->dentries[found].file_type;
	dentry->inode_num = file_sys->dentries[found].inode_num;
	return 0;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry)
{
	uint32_t i;
	
	//Return failure if index is out of bounds
	if (index >= file_sys->num_dentries)
		return -1;
	
	for (i = 0; i < FILENAME_LEN; i ++)
		dentry->file_name[i] = file_sys->dentries[index].file_name[i];
	dentry->file_type = file_sys->dentries[index].file_type;
	dentry->inode_num = file_sys->dentries[index].inode_num;
	return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	inode_t * node;
	dblock_t * dblocks;
	dblock_t * dblock;
	uint32_t dblock_page;
	uint32_t i;
		
	//Return failure if index is out of bounds
	if (inode >= file_sys->num_inodes)
		return -1;
	
	node = get_inode(inode);
	
	//The starting data block page
	dblock_page = offset / DATABLOCK_SIZE;
	
	//Check bounds
	if (dblock_page >= node->len)
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
			dblock_page++;
			
			//Double check for end of file
			if (dblock_page == node->len)
				return 0;
			
			//Double check bounds
			else if (dblock_page > node->len)
				return -1;
			if (node->dblocks[dblock_page] >= file_sys->num_blocks)
				return -1;
			dblock = &dblocks[node->dblocks[dblock_page]];
			offset = 0;
		}
	}
	return 0;
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
		uint8_t buf[40];
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
		printf("Reading first 160 Bytes...\n");
		read_data(dentry.inode_num, 0, buf, 40);
		printf("%s\n", buf);
		
		read_data(dentry.inode_num, 40, buf, 40);
		printf("%s\n", buf);
		
		read_data(dentry.inode_num, 80, buf, 40);
		printf("%s\n", buf);
		
		read_data(dentry.inode_num, 120, buf, 40);
		printf("%s\n", buf);
		
		for(j = 0; j < 1000000000; j ++);
	}
}
