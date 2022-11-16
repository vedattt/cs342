#include "workload_params.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

void print_arguments(struct workload_param_store* param_store) {
#ifdef DEBUG_LOG
    printf("param_mode: %d (C: %d, F: %d)\n", param_store->param_mode, WL_PARAM_COMMANDLINE, WL_PARAM_FILE);

    printf("common params -> rqLen: %d, ALLP: %d, OUTMODE: %d, output_file_name: %s\n",
            param_store->common_params->max_runqueue_length,
            param_store->common_params->total_process_count,
            param_store->common_params->debug_output_mode,
            param_store->common_params->output_file_name);

    switch (param_store->param_mode) {
        case WL_PARAM_COMMANDLINE:
            printf("command line params ->\n");
            printf("\tminPrio: %d, maxPrio: %d\n",
                    param_store->commandline_params->min_priority,
                    param_store->commandline_params->max_priority);
            printf("\tdistPL: %d, avgPL: %d, minPL: %d, maxPL: %d\n",
                    param_store->commandline_params->process_length_distribution,
                    param_store->commandline_params->average_process_length,
                    param_store->commandline_params->min_process_length,
                    param_store->commandline_params->max_process_length);
            printf("\tdistIAT: %d, avgIAT: %d, minIAT: %d, maxIAT: %d\n",
                    param_store->commandline_params->interarrival_time_distribution,
                    param_store->commandline_params->average_interarrival_time,
                    param_store->commandline_params->min_interarrival_time,
                    param_store->commandline_params->max_interarrival_time);
            break;
        case WL_PARAM_FILE:
            printf("file params ->\n");
            printf("\tprocesses ->\n");
            struct process_params* process_node = param_store->file_params->processes;
            while (process_node) {
                printf("length: %d, priority: %d, iat: %d\n",
                        process_node->length,
                        process_node->priority,
                        process_node->interarrival_time);
                process_node = process_node->next_process;
            }
            break;
    }
#endif /* DEBUG_LOG */
}

enum distribution_method parse_distribution_method(char* str) {
    if (strcmp(str, "fixed")) return FIXED_DIST;
    if (strcmp(str, "uniform")) return UNIFORM_DIST;
    if (strcmp(str, "exponential")) return EXPONENTIAL_DIST;

    printf("Invalid distribution method string: '%s'\n", str);
    exit(1);
}

void parse_common_arguments(struct workload_param_store* param_store, char* args[], int* i) {
    param_store->common_params = malloc(sizeof *param_store->common_params);
    struct workload_common_params* common_params = param_store->common_params;

    common_params->max_runqueue_length = atoi(args[(*i)++]);
    common_params->total_process_count = atoi(args[(*i)++]);
    common_params->debug_output_mode = atoi(args[(*i)++]);
}

void parse_commandline_arguments(struct workload_param_store* param_store, char* args[], int* i) {
    param_store->commandline_params = malloc(sizeof *param_store->commandline_params);
    struct workload_commandline_params* cl_params = param_store->commandline_params;

    cl_params->min_priority = atoi(args[(*i)++]);
    cl_params->max_priority = atoi(args[(*i)++]);

    cl_params->process_length_distribution = parse_distribution_method(args[(*i)++]);
    cl_params->average_process_length = atoi(args[(*i)++]);
    cl_params->min_process_length = atoi(args[(*i)++]);
    cl_params->max_process_length = atoi(args[(*i)++]);

    cl_params->interarrival_time_distribution = parse_distribution_method(args[(*i)++]);
    cl_params->average_interarrival_time = atoi(args[(*i)++]);
    cl_params->min_interarrival_time = atoi(args[(*i)++]);
    cl_params->max_interarrival_time = atoi(args[(*i)++]);

    parse_common_arguments(param_store, args, i);
}

void parse_file_arguments(struct workload_param_store* param_store, char* args[], int* i) {
    param_store->file_params = malloc(sizeof *param_store->file_params);
    struct workload_file_params* file_params = param_store->file_params;

    parse_common_arguments(param_store, args, i);

    char* input_file_name = args[(*i)++];
    FILE* fp = fopen(input_file_name, "r");
    if (!fp) {
        printf("Failed to open input file '%s'\n", input_file_name);
        exit(1);
    }

    struct process_params dummy_node;
    struct process_params* node = &dummy_node;
    int process_length = 0, process_priority = 0, interarrival_time = 0;
    while (fscanf(fp, " PL %d %d", &process_length, &process_priority) == 2) {
        node->next_process = malloc(sizeof *node->next_process);
        node->next_process->length = process_length;
        node->next_process->priority = process_priority;

        if (fscanf(fp, " IAT %d", &interarrival_time) == 1) {
            node->next_process->interarrival_time = interarrival_time;
        } else {
            node->next_process->interarrival_time = 0;
        }

        node = node->next_process;
    }
    file_params->processes = dummy_node.next_process;
}

struct workload_param_store* parse_arguments(int argc, char* args[]) {
    struct workload_param_store* param_store = malloc(sizeof *param_store);
    int i = 1;

    switch (args[i++][0]) {
        case 'C':
            param_store->param_mode = WL_PARAM_COMMANDLINE;
            parse_commandline_arguments(param_store, args, &i);
            break;
        case 'F':
            param_store->param_mode = WL_PARAM_FILE;
            parse_file_arguments(param_store, args, &i);
            break;
        default:
            printf("Invalid parameter mode given\n");
            exit(1);
    }

    if (i != argc) {
        char* output_file_name = args[i++];
        param_store->common_params->output_file_name = malloc(strlen(output_file_name) + 1);
        strcpy(param_store->common_params->output_file_name, output_file_name);
    }

    print_arguments(param_store);
    return param_store;
}

void free_arguments(struct workload_param_store* param_store) {
    free(param_store->common_params->output_file_name);
    free(param_store->common_params);
    
    switch (param_store->param_mode) {
        case WL_PARAM_COMMANDLINE:
            free(param_store->commandline_params);
            break;
        case WL_PARAM_FILE:
            {
                struct process_params* param_node = param_store->file_params->processes;
                while (param_node) {
                    struct process_params* temp_node = param_node;
                    param_node = param_node->next_process;
                    free(temp_node);
                }

                free(param_store->file_params);
                break;
            }
    }
}

struct workload_param_process_iterator init_params_iterator(struct workload_param_store* param_store) {
    struct workload_param_process_iterator iterator = {
        .param_store = param_store,
        .iteration_count = 0,
        .next_process_params = param_store->file_params->processes
    };
    return iterator;
}

int has_next_process(struct workload_param_process_iterator params_iterator) {
    return params_iterator.param_store->common_params->total_process_count != params_iterator.iteration_count;
}

int get_distributed_process_length(struct workload_param_store* param_store) {
    switch (param_store->commandline_params->process_length_distribution) {
        case FIXED_DIST:
            return param_store->commandline_params->average_process_length;
        case UNIFORM_DIST:
            return get_random_in_range(param_store->commandline_params->min_process_length,
                    param_store->commandline_params->max_process_length);
        case EXPONENTIAL_DIST:
            return get_exponential_random(param_store->commandline_params->average_process_length,
                    param_store->commandline_params->min_process_length,
                    param_store->commandline_params->max_process_length);
    }
}

int get_distributed_interarrival_time(struct workload_param_store* param_store) {
    switch (param_store->commandline_params->interarrival_time_distribution) {
        case FIXED_DIST:
            return param_store->commandline_params->average_interarrival_time;
        case UNIFORM_DIST:
            return get_random_in_range(param_store->commandline_params->min_interarrival_time,
                    param_store->commandline_params->max_interarrival_time);
        case EXPONENTIAL_DIST:
            return get_exponential_random(param_store->commandline_params->average_interarrival_time,
                    param_store->commandline_params->min_interarrival_time,
                    param_store->commandline_params->max_interarrival_time);
    }
}

struct process_params get_next_process(struct workload_param_process_iterator params_iterator) {
    switch (params_iterator.param_store->param_mode) {
        case WL_PARAM_COMMANDLINE:
            {
                struct process_params random_process = {
                    .priority = get_random_in_range(params_iterator.param_store->commandline_params->min_priority,
                        params_iterator.param_store->commandline_params->max_priority),
                    .length = get_distributed_process_length(params_iterator.param_store),
                    .interarrival_time = get_distributed_interarrival_time(params_iterator.param_store)
                };
                return random_process;
            }
        case WL_PARAM_FILE:
            {
                struct process_params* temp_node = params_iterator.next_process_params;
                params_iterator.next_process_params = params_iterator.next_process_params->next_process;
                return *temp_node;
            }
    }
}

