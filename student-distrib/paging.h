#ifndef _PAGING_H
#define _PAGING_H

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
