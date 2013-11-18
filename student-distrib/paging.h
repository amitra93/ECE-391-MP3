#ifndef _PAGING_H
#define _PAGING_H

#define FIRST_20_BITS 0xFFFFF000 /*first 20 bits of 32 bit value used for mask*/
#define FIRST_10_BITS 0xFFC00000 /*first 10 bits of 32 bit value used for mask*/
#define KERNEL_LOCATION   0x400000 /*starting location in memory where kernel resides*/
#define NUM_PAGES 1024 /*number of pages in a page directory*/
#define SIZE_4KB_PAGE 0x1000 /*this size of a 4kb page (4KB!)*/
#define SIZE_4MB_PAGE 0x400000 /*this size of a 4mb page (4MB!)*/
#define CR4_4MB_PAGE_BIT 0x10 /*bit of CR4 control register that enables 4mb paging*/
#define CR0_PAGE_ENBL_BIT 0x80000000 /*bit of CR0 control register that enables paging*/
#define SUPERVISOR_RW_PRESENT 0x3 /*bits necessary to set supervisor to priv., r/w adn pressent*/
#define RW_PRESENT 0x3
#define PAGE_SIZE_BIT 0x80 /*bit used to set page to 4MB */

#ifndef ASM

#include "types.h"

extern void paging_init();

extern int32_t clear_pde(uint32_t virt_addr);
extern int32_t clear_pte(uint32_t virt_addr);
extern int32_t set_pde(uint32_t virt_addr);
extern int32_t set_pte(uint32_t virt_addr);
extern int32_t map_page_directory(uint32_t phys_addr, uint32_t virt_addr, uint8_t size, uint8_t user);
extern int32_t map_page_table(uint32_t phys_addr, uint32_t virt_addr, uint8_t user);
extern int32_t map_page_table_from_index(uint32_t phys_addr, uint32_t pd_index, uint32_t pt_index, uint8_t user);

extern unsigned int page_directory;
extern unsigned int page_table;

extern unsigned int * pd;
extern unsigned int * pt;

#endif /* ASM */

#endif /* _PAGING_H */
