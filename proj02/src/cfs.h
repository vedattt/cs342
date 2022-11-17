#ifndef __GUARD_CFS_H
#define __GUARD_CFS_H

#include "process.h"

#define SCHED_LATENCY 100
#define MIN_GRANULARITY 10

struct runqueue_node {
    struct process_control_block* pcb;
    struct runqueue_node* next;
};

struct runqueue {
    struct runqueue_node* head;
    int count;
};

struct runqueue* runqueue_init();
void runqueue_free(struct runqueue* rq);
void runqueue_add_pcb(struct runqueue* rq, struct process_control_block* pcb);
void runqueue_remove_pcb(struct runqueue* rq, struct process_control_block* pcb);

int runqueue_timeslice(struct runqueue* rq, int priority);
double runqueue_vruntime(struct runqueue* rq, int priority, int actual_runtime);
struct process_control_block* runqueue_smallest_pcb(struct runqueue* rq);

#endif // !__GUARD_CFS_H
