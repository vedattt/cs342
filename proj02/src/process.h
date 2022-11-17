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

void* start_process_thread(void*);

#endif // !__GUARD_PROCESS_H
