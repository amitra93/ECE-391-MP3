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

sched_t schedular = {
	.task_vector = 0,
	.max_tasks = 6,
	.num_tasks = 0,
	.cur_task = -1
};

int32_t create_task();
int32_t end_task(uint32_t pid);
int32_t set_cur_task(uint32_t pid);
task_t* get_cur_task();
int32_t switch_task(uint32_t pid);
#endif /* _SCHED_H */
