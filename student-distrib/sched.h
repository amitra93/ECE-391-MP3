#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"
#include "task.h"

typedef struct sched_t {
	uint32_t task_vector;
	uint32_t max_tasks;
	uint32_t num_tasks;
	int32_t cur_task;
	
	int32_t ptree_tasks[32];
	int32_t ptree_vector;
	uint32_t max_ptrees;
	uint32_t num_ptrees;
	int32_t cur_ptree;
	
} sched_t;

extern sched_t schedular;

int32_t tasks_init();
int32_t create_task(const uint8_t * fname, const uint8_t args [128]);
int32_t end_task(int32_t pid);
int32_t set_cur_task(int32_t pid);
int32_t set_cur_task_state(task_state state);
task_t* get_cur_task();
task_state get_cur_task_state();
int32_t switch_task(int32_t pid);

#endif /* _SCHED_H */
