#ifndef _PAGING_H
#define _PAGING_H

#ifndef ASM

#include "types.h"

extern void paging_init();

extern int32_t map_page_directory(uint32_t phys_addr, uint32_t virt_addr, uint8_t size);
extern int32_t map_page_table(uint32_t phys_addr, uint32_t virt_addr);
extern int32_t map_page_table_from_index(uint32_t phys_addr, uint32_t pd_index, uint32_t pt_index);

#endif /* ASM */

#endif /* _PAGING_H */
