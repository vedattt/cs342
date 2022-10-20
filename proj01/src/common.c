#include "common.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

long long _start_time;

long long get_microseconds() {
    struct timeval time;
    gettimeofday(&time, 0);
    return time.tv_sec * 1000000 + time.tv_usec;
}

void start_time() {
#ifdef DEBUG_LOG
    _start_time = get_microseconds();
#endif /* DEBUG_LOG */
}

void end_time() {
#ifdef DEBUG_LOG
    printf("microseconds elapsed: %lld\n", get_microseconds() - _start_time);
#endif /* DEBUG_LOG */
}

void print_cli_args(struct command_line_args* args) {
#ifdef DEBUG_LOG
    printf("msgsize: '%d', out: '%s', incount: '%d'\n", args->msg_size, args->out_file_name, args->in_file_count);
    printf("input file names:\n");
    for (int i = 0; i != args->in_file_count; i++) {
        printf("\t- %d: %s\n", i, args->in_file_names[i]);
    }
#endif /* DEBUG_LOG */
}

struct command_line_args parse_arguments(int argc, char* args[], int parse_msg_size) {
    if (argc < 4) {
        printf("Something is wrong with the commandline arguments\n");
        exit(1);
    }

    struct command_line_args parsed_args;
    int i = 1;

    parsed_args.msg_size = -1;
    if (parse_msg_size) {
        parsed_args.msg_size = atoi(args[i++]);
        
        const int valid_msg_sizes[6] = { 128, 256, 512, 1024, 2048, 4096 };
        int msg_size_is_valid = 0;
        for (int j = 0; j != sizeof(valid_msg_sizes) / sizeof(valid_msg_sizes[0]); j++) {
            if (parsed_args.msg_size == valid_msg_sizes[j])
                msg_size_is_valid = 1;
        }
        
        if (!msg_size_is_valid) {
            printf("msg_size was given an invalid value\n");
            exit(1);
        }
    }

    parsed_args.out_file_name = args[i++];
    parsed_args.in_file_count = atoi(args[i++]);

    for (int j = 0; j != parsed_args.in_file_count; j++) {
        char* file_name = args[i + j];
        if (!file_name) {
            printf("Passed in less input file names than specified\n");
            exit(1);
        }
        parsed_args.in_file_names[j] = file_name;
    }

    print_cli_args(&parsed_args);
    return parsed_args;
}

void str_toupper(char* str) {
    char* c = str;
    while (*c) {
        *c = toupper((unsigned char) *c);
        c++;
    }
}

struct word_count_array* init_word_count_array() {
    struct word_count_array* pair_array = malloc(sizeof *pair_array);
    pair_array->allocated_size = 20;
    pair_array->used_size = 0;
    pair_array->pairs = malloc(pair_array->allocated_size * sizeof *pair_array);
    return pair_array;
}

void free_word_count_array(struct word_count_array* pair_array) {
    for (size_t i = 0; i != pair_array->used_size; i++) {
        free(pair_array->pairs[i].word);
    }
    free(pair_array->pairs);
    free(pair_array);
}

void print_pairs(struct word_count_array* pair_array) {
#ifdef DEBUG_LOG
    printf("pairs:\n");
    for (int i = 0; i != pair_array->used_size; i++) {
        printf("'%s', %d\n", pair_array->pairs[i].word, pair_array->pairs[i].count);
    }
#endif /* DEBUG_LOG */
}

int word_count_pair_comparer(const void* a, const void* b) {
    return strcmp(((struct word_count_pair*)a)->word, ((struct word_count_pair*)b)->word);
}

void sort_word_count_array(struct word_count_array* pair_array) {
    qsort(pair_array->pairs, pair_array->used_size, sizeof *pair_array->pairs, word_count_pair_comparer);
    print_pairs(pair_array);
}

void shrink_array_to_fit(struct word_count_array* pair_array) {
    pair_array->pairs = realloc(pair_array->pairs, pair_array->used_size * sizeof *pair_array->pairs);
    pair_array->allocated_size = pair_array->used_size;
}

void realloc_array_if_full(struct word_count_array* pair_array) {
    if (pair_array->allocated_size == pair_array->used_size) {
        pair_array->allocated_size *= 2;
        pair_array->pairs = realloc(pair_array->pairs, pair_array->allocated_size * sizeof *pair_array->pairs);
    }
}

int try_increment_existing_word(struct word_count_array* pair_array, char* word, int count) {
    for (size_t i = 0; i != pair_array->used_size; i++) {
        if (strcmp(pair_array->pairs[i].word, word) == 0) {
            pair_array->pairs[i].count += count;
            return 1;
        }
    }
    return 0;
}

void insert_word_pair(struct word_count_array* pair_array, char* word, int count) {
    realloc_array_if_full(pair_array);

    struct word_count_pair* pair = &pair_array->pairs[pair_array->used_size++];
    pair->word = malloc(strlen(word) + 1);
    strcpy(pair->word, word);
    pair->count = count;
}

void increment_word(struct word_count_array* pair_array, char* word, int count) {
    if (!try_increment_existing_word(pair_array, word, count)) {
        insert_word_pair(pair_array, word, count);
    }
}

void add_array_to_array(struct word_count_array* total_array, const struct word_count_array* other_array) {
    for (size_t i = 0; i != other_array->used_size; i++) {
        increment_word(total_array, other_array->pairs[i].word, other_array->pairs[i].count);
    }
}

struct word_count_array* parse_in_file(char* in_file_name) {
    FILE* fp = fopen(in_file_name, "r");
    if (!fp) {
        printf("Failed to open input file '%s'\n", in_file_name);
        exit(2);
    }

    struct word_count_array* pair_array = init_word_count_array();
    char word[64];
    while (fscanf(fp, " %63s", word) == 1) {
        str_toupper(word);
        increment_word(pair_array, word, 1);
    }
    fclose(fp); 
    shrink_array_to_fit(pair_array);

    print_pairs(pair_array); // Only works with debug flag
    return pair_array;
}

void write_output_file(struct word_count_array* pair_array, char* out_file_name) {
    sort_word_count_array(pair_array);

    FILE* fp = fopen(out_file_name, "w");
    if (!fp) {
        printf("Failed to open output file '%s'\n", out_file_name);
        exit(3);
    }

    for (size_t i = 0; i != pair_array->used_size; i++) {
        fprintf(fp, "%s %d\n", pair_array->pairs[i].word, pair_array->pairs[i].count);
    }
    fclose(fp);
}

