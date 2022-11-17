#include "generator.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "process.h"
#include "scheduler.h"
#include "workload_params.h"
#include "utility.h"

pthread_t create_generator_thread(struct workload_param_store* param_store) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t tid;
    pthread_create(&tid, &attr, start_generator_thread, param_store);

    return tid;
}

void* start_generator_thread(void* argument) {
    struct workload_param_store* workload_params = argument;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int i = 1;
    struct workload_param_process_iterator iterator = init_params_iterator(workload_params);
    while (has_next_process(&iterator)) {
        struct process_params process_params = get_next_process(&iterator);

        struct process_thread_arguments* process_thread_args = malloc(sizeof *process_thread_args);

        struct process_control_block* pcb = malloc(sizeof *pcb);
        pcb->priority = process_params.priority;
        pcb->process_length = process_params.length;
        pcb->cpu_time = 0;
        pcb->vruntime = 0;
        pcb->pid = i++;
        pcb->state = PROCESS_READY;
        pthread_mutex_init(&pcb->mutex, NULL);
        pthread_cond_init(&pcb->cond_var, NULL);

        process_thread_args->pcb = pcb;
        process_thread_args->workload_params = workload_params;

        pthread_t tid;
        pthread_create(&tid, &attr, start_process_thread, process_thread_args);
        pcb->tid = tid;

        do {
            usleep(process_params.interarrival_time * 1000);
        } while (get_runqueue_item_count() == workload_params->common_params->max_runqueue_length);
    }

    if (workload_params->common_params->debug_output_mode == DBG_MODE_FULL)
        printf("Generator thread is exiting\n");

    pthread_exit(0);
}

