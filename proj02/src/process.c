#include "process.h"
#include "scheduler.h"
#include "workload_params.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* start_process_thread(void* argument) {
    struct process_thread_arguments* process_thread_args = argument;
    struct process_control_block* pcb = process_thread_args->pcb;

    if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
        printf("new process created (pid %d)\n", pcb->pid);
    
    while (1) {
        if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
            printf("adding pcb to runqueue (pid %d)\n", pcb->pid);
        
        add_pcb_to_runqueue(pcb);
        signal_scheduler();

        pthread_mutex_lock(&pcb->mutex);
        while (pcb->state != PROCESS_RUNNING)
            pthread_cond_wait(&pcb->cond_var, &pcb->mutex);

        remove_pcb_from_runqueue(pcb);

        int remaining_time = pcb->process_length - pcb->cpu_time;
        int timeslice = get_timeslice_for_pcb(pcb);
        int cpu_time = (remaining_time > timeslice ? timeslice : remaining_time);
        if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
            printf("running process for %d ms in CPU (pid %d)\n", cpu_time, pcb->pid);

        pthread_mutex_unlock(&pcb->mutex);

        usleep(cpu_time * 1000);
        pcb->cpu_time += cpu_time;
        pcb->vruntime += convert_to_vruntime_for_pcb(pcb, cpu_time);

        if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
            printf("process ran out timeslice (pid %d)\n", pcb->pid);

        if (pcb->process_length <= pcb->cpu_time) {
            break;
        }
    }
    notify_process_termination();
    signal_scheduler();

    if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
        printf("process is terminating (pid %d)\n", pcb->pid);

    pthread_mutex_destroy(&pcb->mutex);
    pthread_cond_destroy(&pcb->cond_var);
    free(pcb);
    pthread_exit(0);
}

