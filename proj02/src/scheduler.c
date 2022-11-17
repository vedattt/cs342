#include "scheduler.h"
#include "cfs.h"
#include "process.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

struct runqueue* rq;

int have_running_process = 0;
int waiting_for_new_process = 1;
pthread_mutex_t mutex_need_scheduling;
pthread_cond_t cond_var_need_scheduling;
int remaining_process_count;

pthread_mutex_t mutex_runqueue;


pthread_t create_scheduler_thread(struct workload_param_store* param_store) {
    rq = runqueue_init();

    pthread_mutex_init(&mutex_need_scheduling, NULL);
    pthread_cond_init(&cond_var_need_scheduling, NULL);

    pthread_mutex_init(&mutex_runqueue, NULL);

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
        while (have_running_process || waiting_for_new_process)
            pthread_cond_wait(&cond_var_need_scheduling, &mutex_need_scheduling);

        lock_rq();
        struct process_control_block* pcb = runqueue_smallest_pcb(rq);
        unlock_rq();

        if (pcb != NULL) {
            if (workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
                printf("selected process for CPU (pid %d, vruntime %f)\n", pcb->pid, pcb->vruntime);
            signal_process(pcb);
            have_running_process = 1;
        } else {
            waiting_for_new_process = 1;
        }

        pthread_mutex_unlock(&mutex_need_scheduling);
    }

    if (workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
        printf("Scheduler thread is exiting\n");

    pthread_mutex_destroy(&mutex_need_scheduling);
    pthread_cond_destroy(&cond_var_need_scheduling);
    pthread_mutex_destroy(&mutex_runqueue);
    runqueue_free(rq);
    rq = NULL;
    pthread_exit(0);
}

int get_runqueue_item_count() {
    lock_rq();
    int count = rq == NULL ? 0 : rq->count;
    unlock_rq();
    return count;
}

void add_pcb_to_runqueue(struct process_control_block* pcb) {
    runqueue_add_pcb(rq, pcb);
}

void remove_pcb_from_runqueue(struct process_control_block* pcb) {
    runqueue_remove_pcb(rq, pcb);
}

int get_timeslice_for_pcb(struct process_control_block* pcb) {
    return runqueue_timeslice(rq, pcb->priority);
}

void update_pcb_timings(struct process_control_block* pcb, int cpu_time) {
    pcb->cpu_time += cpu_time;
    pcb->vruntime += runqueue_vruntime(rq, pcb->priority, cpu_time);
}

void signal_scheduler_new_process() {
    pthread_mutex_lock(&mutex_need_scheduling);
    if (!have_running_process)
        waiting_for_new_process = 0;
    pthread_cond_signal(&cond_var_need_scheduling);
    pthread_mutex_unlock(&mutex_need_scheduling);
}

void signal_scheduler_terminated_process() {
    pthread_mutex_lock(&mutex_need_scheduling);
    have_running_process = 0;
    pthread_cond_signal(&cond_var_need_scheduling);
    pthread_mutex_unlock(&mutex_need_scheduling);
}

void notify_process_termination() {
    lock_rq();
    remaining_process_count--;
    unlock_rq();
}

void lock_rq() {
    pthread_mutex_lock(&mutex_runqueue);
}

void unlock_rq() {
    pthread_mutex_unlock(&mutex_runqueue);
}

