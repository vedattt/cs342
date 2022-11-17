#include "generator.h"
#include "process.h"
#include "scheduler.h"
#include "utility.h"
#include "workload_params.h"
#include <stdio.h>
#include <stdlib.h>

struct workload_param_store* workload_params;

void print_table() {
    struct process_statistics* stats = get_process_stats();
    double avg_waiting_time = 0;

    if (workload_params->common_params->output_file_name != NULL) {
        freopen(workload_params->common_params->output_file_name, "w", stdout);
    }

    printf("%-6s%-7s%-7s%-6s%-7s%-7s%-7s%-4s\n", "pid", "arv", "dept", "prio", "cpu", "waitr", "turna", "cs");
    for (int i = 0; i != workload_params->common_params->total_process_count; i++) {
        struct process_statistics* current = &stats[i];
        avg_waiting_time += current->waiting_time;
        printf("%-6d%-7d%-7d%-6d%-7d%-7d%-7d%-4d\n",
                current->pid,
                current->arrival_time,
                current->departure_time,
                current->priority,
                current->cpu_time,
                current->waiting_time,
                current->turnaround_time,
                current->context_switch_count);
    }

    avg_waiting_time /= workload_params->common_params->total_process_count;
    printf("avg waiting time: %f\n", avg_waiting_time);

    free(stats);
}

int main(int argc, char* argv[]) {
    workload_params = parse_arguments(argc, argv);

    init_process_stats_array(workload_params->common_params->total_process_count);
    start_simulation_time();
    pthread_t scheduler_tid = create_scheduler_thread(workload_params);
    pthread_t generator_tid = create_generator_thread(workload_params);

    pthread_join(scheduler_tid, NULL);
    pthread_join(generator_tid, NULL);

    print_table();

    free_arguments(workload_params);
    return 0;
}

