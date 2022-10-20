#include "common.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct command_line_args cli_args;
struct word_count_array* pair_array;
struct word_count_array* sub_arrays[MAX_IN_FILE_COUNT];

void* run_child_actions(void* child_count) {
    int i = *(int*)child_count;
    sub_arrays[i] = parse_in_file(cli_args.in_file_names[i]);

    free(child_count);
    pthread_exit(0);
}

void await_worker_threads() {
    pthread_t threads[MAX_IN_FILE_COUNT];

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for (int i = 0; i != cli_args.in_file_count; i++) {
        int *i_ptr = malloc(sizeof *i_ptr);
        *i_ptr = i;
        pthread_t tid;
        pthread_create(&tid, &attr, run_child_actions, i_ptr);
        threads[i] = tid;
    }

    for (int i = 0; i != cli_args.in_file_count; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char* argv[]) {
    start_time();

    cli_args = parse_arguments(argc, argv, 0);

    pair_array = init_word_count_array();

    await_worker_threads();

    for (int i = 0; i != cli_args.in_file_count; i++) {
        add_array_to_array(pair_array, sub_arrays[i]);
        free_word_count_array(sub_arrays[i]);
    }

    write_output_file(pair_array, cli_args.out_file_name);
    free_word_count_array(pair_array);

    end_time();
    return 0;
}

