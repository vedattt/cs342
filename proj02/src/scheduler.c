#include "scheduler.h"
#include "cfs.h"
#include "process.h"
#include <pthread.h>
#include <stdio.h>

struct runqueue* rq;

int need_scheduling = 0;
pthread_mutex_t mutex_need_scheduling;
pthread_cond_t cond_var_need_scheduling;

pthread_mutex_t mutex_runqueue;

int remaining_process_count;
pthread_mutex_t mutex_remaining_process_count;

pthread_t create_scheduler_thread(struct workload_param_store* param_store) {
    rq = runqueue_init();

    pthread_mutex_init(&mutex_need_scheduling, NULL);
    pthread_cond_init(&cond_var_need_scheduling, NULL);

    pthread_mutex_init(&mutex_runqueue, NULL);

    pthread_mutex_init(&mutex_remaining_process_count, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t tid;
    pthread_create(&tid, &attr, start_scheduler_thread, param_store);

    return tid;
}

void signal_process(struct process_control_block* pcb) {
    pthread_mutex_lock(&pcb->mutex);
    pcb->state = PROCESS_RUNNING;
    pthread_cond_signal(&pcb->cond_var);
    pthread_mutex_unlock(&pcb->mutex);
}

void* start_scheduler_thread(void* argument) {
    struct workload_param_store* workload_params = argument;

    remaining_process_count = workload_params->common_params->total_process_count;
    while (remaining_process_count) {
        pthread_mutex_lock(&mutex_need_scheduling);
        while (!need_scheduling)
            pthread_cond_wait(&cond_var_need_scheduling, &mutex_need_scheduling);

        pthread_mutex_lock(&mutex_runqueue);
        struct process_control_block* pcb = runqueue_smallest_pcb(rq);
        pthread_mutex_unlock(&mutex_runqueue);

        if (pcb != NULL) {
            if (workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
                printf("selected process for CPU (pid %d)\n", pcb->pid);
            signal_process(pcb);
        }

        pthread_mutex_unlock(&mutex_need_scheduling);

        if (workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
            printf("remaining process count: %d\n", remaining_process_count);
    }

    if (workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
        printf("Scheduler thread is exiting\n");

    pthread_mutex_destroy(&mutex_need_scheduling);
    pthread_cond_destroy(&cond_var_need_scheduling);
    pthread_mutex_destroy(&mutex_runqueue);
    pthread_mutex_destroy(&mutex_remaining_process_count);
    runqueue_free(rq);
    pthread_exit(0);
}

int get_runqueue_item_count() {
    pthread_mutex_lock(&mutex_runqueue);
    int count = rq->count;
    pthread_mutex_unlock(&mutex_runqueue);
    return count;
}

void add_pcb_to_runqueue(struct process_control_block* pcb) {
    pthread_mutex_lock(&mutex_runqueue);
    runqueue_add_pcb(rq, pcb);
    pthread_mutex_unlock(&mutex_runqueue);
}

void remove_pcb_from_runqueue(struct process_control_block* pcb) {
    pthread_mutex_lock(&mutex_runqueue);
    runqueue_remove_pcb(rq, pcb);
    pthread_mutex_unlock(&mutex_runqueue);
}

int get_timeslice_for_pcb(struct process_control_block* pcb) {
    pthread_mutex_lock(&mutex_runqueue);
    int timeslice = runqueue_timeslice(rq, pcb->priority);
    pthread_mutex_unlock(&mutex_runqueue);
    return timeslice;
}

double convert_to_vruntime_for_pcb(struct process_control_block* pcb, int actual_runtime) {
    pthread_mutex_lock(&mutex_runqueue);
    double vruntime = runqueue_vruntime(rq, pcb->priority, actual_runtime);
    pthread_mutex_unlock(&mutex_runqueue);
    return vruntime;
}

void signal_scheduler() {
    pthread_mutex_lock(&mutex_need_scheduling);
    need_scheduling = 1;
    pthread_cond_signal(&cond_var_need_scheduling);
    pthread_mutex_unlock(&mutex_need_scheduling);
}

void notify_process_termination() {
    pthread_mutex_lock(&mutex_remaining_process_count);
    remaining_process_count--;
    pthread_mutex_unlock(&mutex_remaining_process_count);
}

