#ifndef __GUARD_SCHEDULER_H
#define __GUARD_SCHEDULER_H

#include <pthread.h>
#include "process.h"
#include "workload_params.h"

pthread_t create_scheduler_thread(struct workload_param_store* param_store);
void* start_scheduler_thread(void*);

int get_runqueue_item_count();
void add_pcb_to_runqueue();
void remove_pcb_from_runqueue();
int get_timeslice_for_pcb(struct process_control_block* pcb);
void update_pcb_timings(struct process_control_block* pcb, int cpu_time);

void signal_scheduler_new_process();
void signal_scheduler_terminated_process();
void notify_process_termination();

void set_cpu_occupied(int occupied);

void lock_rq();
void unlock_rq();

#endif // !__GUARD_SCHEDULER_H
