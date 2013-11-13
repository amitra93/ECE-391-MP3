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

void test_file_system();

int32_t init_file_system(uint32_t * start_addr);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t write_file(uint32_t offset, const uint8_t* buf, uint32_t length);
int32_t close_file();
int32_t open_file();


file_sys_t * file_sys;

#endif /* ASM */

#endif /* _FILE_SYS_H */
