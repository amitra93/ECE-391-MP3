#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"
#include "task.h"

typedef struct sched_t {
	uint32_t task_vector;
	uint32_t max_tasks;
	uint32_t num_tasks;
	int32_t cur_task;
} sched_t;

extern sched_t schedular;

int32_t create_task(const uint8_t * fname, const uint8_t args [128]);
int32_t end_task(uint32_t pid);
int32_t set_cur_task(uint32_t pid);
task_t* get_cur_task();
int32_t switch_task(uint32_t pid);
#endif /* _SCHED_H */
