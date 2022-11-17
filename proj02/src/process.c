#include "process.h"
#include "scheduler.h"
#include "utility.h"
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

    int context_switch_count = 0;
    int arrival_time = get_time_since_t0();
    
    while (pcb->process_length > pcb->cpu_time) {
        if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
            printf("adding pcb to runqueue (pid %d)\n", pcb->pid);
        
        add_pcb_to_runqueue(pcb);
        signal_scheduler();

        pthread_mutex_lock(&pcb->mutex);
        while (pcb->state != PROCESS_RUNNING)
            pthread_cond_wait(&pcb->cond_var, &pcb->mutex);

        set_cpu_occupied(1);
        remove_pcb_from_runqueue(pcb);

        int remaining_time = pcb->process_length - pcb->cpu_time;
        int timeslice = get_timeslice_for_pcb(pcb);
        int cpu_time = (remaining_time > timeslice ? timeslice : remaining_time);
        if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
            printf("running process for %d ms in CPU (pid %d)\n", cpu_time, pcb->pid);

        pthread_mutex_unlock(&pcb->mutex);

        if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_LIGHT)
            printf("%d %d %s %d\n", get_time_since_t0(), pcb->pid, "RUNNING", cpu_time);

        usleep(cpu_time * 1000);
        pcb->cpu_time += cpu_time;
        pcb->vruntime += convert_to_vruntime_for_pcb(pcb, cpu_time);
        context_switch_count++;
        set_cpu_occupied(0);

        if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
            printf("process ran out timeslice (pid %d)\n", pcb->pid);
    }
    notify_process_termination();
    signal_scheduler();

    if (process_thread_args->workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
        printf("process is terminating (pid %d)\n", pcb->pid);

    int departure_time = get_time_since_t0();
    int turnaround_time = departure_time - arrival_time;
    int waiting_time = turnaround_time - pcb->cpu_time; 

    struct process_statistics stats = {
        .pid = pcb->pid,
        .arrival_time = arrival_time,
        .departure_time = departure_time,
        .priority = pcb->priority,
        .cpu_time = pcb->cpu_time,
        .waiting_time = waiting_time,
        .turnaround_time = turnaround_time,
        .context_switch_count = context_switch_count
    };
    send_process_stats(stats);

    pthread_mutex_destroy(&pcb->mutex);
    pthread_cond_destroy(&pcb->cond_var);
    free(pcb);
    pthread_exit(0);
}

