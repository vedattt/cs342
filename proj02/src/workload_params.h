#ifndef __GUARD_WORKLOAD_PARAMS_H
#define __GUARD_WORKLOAD_PARAMS_H

#include <stddef.h>

enum workload_param_mode { WL_PARAM_COMMANDLINE, WL_PARAM_FILE };

enum distribution_method { FIXED_DIST, UNIFORM_DIST, EXPONENTIAL_DIST };

enum debug_output_mode { DBG_MODE_NOTHING = 1, DBG_MODE_LIGHT = 2, DBG_MODE_FULL = 3 };

struct workload_common_params {
    int max_runqueue_length;
    int total_process_count;
    enum debug_output_mode debug_output_mode;
    char* output_file_name;
};

struct workload_commandline_params {
    int min_priority;
    int max_priority;

    enum distribution_method process_length_distribution;
    int average_process_length;
    int min_process_length;
    int max_process_length;

    enum distribution_method interarrival_time_distribution;
    int average_interarrival_time;
    int min_interarrival_time;
    int max_interarrival_time;
};

struct process_params {
    int length;
    int priority;
    int interarrival_time;
    struct process_params* next_process;
};

struct workload_file_params {
    struct process_params* processes;
};

struct workload_param_store {
    enum workload_param_mode param_mode;
    struct workload_common_params* common_params;
    struct workload_commandline_params* commandline_params;
    struct workload_file_params* file_params;
};

struct workload_param_process_iterator {
    struct workload_param_store* param_store;
    struct process_params* next_process_params;
    int iteration_count;
};

struct workload_param_store* parse_arguments(int argc, char* args[]);
void free_arguments(struct workload_param_store* param_store);

struct workload_param_process_iterator init_params_iterator(struct workload_param_store* param_store);
int has_next_process(struct workload_param_process_iterator params_iterator);
struct process_params get_next_process(struct workload_param_process_iterator params_iterator);

#endif // !__GUARD_WORKLOAD_PARAMS_H

