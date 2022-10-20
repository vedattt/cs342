#ifndef __GUARD_COMMON_H
#define __GUARD_COMMON_H

#include <stddef.h>

#define DEBUG_LOG

#define MAX_IN_FILE_COUNT 8

void start_time();
void end_time();

struct command_line_args {
    int msg_size;
    char* out_file_name;
    int in_file_count;
    char* in_file_names[MAX_IN_FILE_COUNT];
};

struct command_line_args parse_arguments(int argc, char* args[], int parse_msg_size);

struct word_count_pair {
    char* word;
    int count;
};

struct word_count_array
{
    struct word_count_pair* pairs;
    size_t allocated_size;
    size_t used_size;
};

struct word_count_array* init_word_count_array();
void free_word_count_array(struct word_count_array* pair_array);
void increment_word(struct word_count_array* pair_array, char* word, int count);
void add_array_to_array(struct word_count_array* total_array, const struct word_count_array* other_array);

struct word_count_array* parse_in_file(char* in_file_name);
void write_output_file(struct word_count_array* pair_array, char* out_file_name);

#endif // !__GUARD_COMMON_H

