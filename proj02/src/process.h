#ifndef __GUARD_PROCESS_H
#define __GUARD_PROCESS_H

#include "workload_params.h"
#include <pthread.h>

enum process_state { PROCESS_READY, PROCESS_RUNNING };

struct process_control_block {
    int pid;
    enum process_state state;
    pthread_t tid;

    int priority;
    int process_length;
    int cpu_time;
    double vruntime;

    pthread_mutex_t mutex;
    pthread_cond_t cond_var;
};

struct process_thread_arguments {
    struct process_control_block* pcb;
    struct workload_param_store* workload_params;
};

struct process_statistics {
    int pid;
    int arrival_time;
    int departure_time;
    int priority;
    int cpu_time;
    int waiting_time;
    int turnaround_time;
    int context_switch_count;
};

void* start_process_thread(void*);

#endif // !__GUARD_PROCESS_H
