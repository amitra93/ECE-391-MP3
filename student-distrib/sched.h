#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"

typedef struct __attribute__((packed)) task_t{
	uint32_t pid;
} task_t;

#endif /* _SCHED_H */
