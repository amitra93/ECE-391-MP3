#ifndef _FILE_SYS_H
#define _FILE_SYS_H

#ifndef ASM

#include "multiboot.h"
#include "types.h"

//Defined in bytes
#define FILESYS_SIZE    4096
#define DATABLOCK_SIZE 	4096 
#define FILENAME_LEN	32
#define DATABLOCK_NUM	1023
#define DENTRY_NUM		63
#define DENTRY_RNUM		24
#define FILESYS_RNUM	52


typedef struct dblock_t {
	uint8_t data[DATABLOCK_SIZE];
} dblock_t;

typedef struct dentry_t {
	uint8_t file_name [FILENAME_LEN];
	uint32_t file_type;
	uint32_t inode_num;
	uint8_t reserved[DENTRY_RNUM];
} dentry_t;

typedef struct inode_t {
	uint32_t len;
	uint32_t dblocks [DATABLOCK_NUM];
} inode_t;

typedef struct file_sys_t {
	uint32_t num_dentries;
	uint32_t num_inodes;
	uint32_t num_blocks;
	uint8_t reserved[FILESYS_RNUM];
	dentry_t dentries [DENTRY_NUM];
} file_sys_t;

//To-do: Fill out arguments
//Return -1 for failure, 0 for success
typedef struct fops_t{
	int32_t (*open)(const uint8_t* filename);
	int32_t (*close)(int32_t fd);
	int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
} fops_t;

typedef struct file_t {
	//Pointer to file operations table
	fops_t * fops;

	//Pointer to dentry
	dentry_t dentry;
	inode_t * inode;
	
	//State if open or closed
	uint32_t offset;
	
	//Flags
	uint32_t flags;
	
} file_t;

void test_file_system();
void test_loader();

int32_t init_file_system(uint32_t * start_addr);
inode_t * get_inode(uint32_t inode);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t write_data(uint32_t offset, const uint8_t* buf, uint32_t length);
int32_t load_program(const uint8_t* fname, uint8_t * pgrm_addr);

int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t directory_close(int32_t fd);
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t directory_open(const uint8_t* directory);

file_sys_t * file_sys;

extern fops_t rtc_fops;
extern fops_t dir_fops;
extern fops_t file_fops;
extern fops_t term_fops;

#endif /* ASM */

#endif /* _FILE_SYS_H */
