#include "common.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define MQ_NAME "/cs342p1"
#define MQ_EOF_VAL -1

struct command_line_args cli_args;

void print_buf(char* buf, size_t size, const char* msg) {
#ifdef DEBUG_LOG
    printf("%s\n", msg);
    for (int i = 0; i != cli_args.msg_size; i++) {
        printf("%02hhx ", buf[i]);
    }
    printf("\n");
#endif /* DEBUG_LOG */
}

void run_child_actions(int child_count, mqd_t mqd) {
    struct word_count_array* pair_array = parse_in_file(cli_args.in_file_names[child_count]);

    char* buf = malloc(cli_args.msg_size);
    size_t i = 0;
    while (i != pair_array->used_size) {
        int pairs_in_message = 0;
        int buf_i = sizeof pairs_in_message;
        memset(buf, 0, cli_args.msg_size);

        for (; i != pair_array->used_size; i++) {
            struct word_count_pair* pair = &pair_array->pairs[i];
            int word_len = strlen(pair->word) + 1;
            if (word_len + sizeof pair->count > cli_args.msg_size - buf_i) {
                i--;
                break;
            }
            pairs_in_message++;

            strcpy(&buf[buf_i], pair->word);
            buf_i += word_len;

            int* freq_loc = (int*)&buf[buf_i];
            *freq_loc = pair->count;
            buf_i += sizeof pair->count;
        }

        int* pairs_count_loc = (int*)&buf[0];
        *pairs_count_loc = pairs_in_message;
        print_buf(buf, cli_args.msg_size, "sent:");
        mq_send(mqd, buf, cli_args.msg_size, 0);
    }

    memset(buf, 0, cli_args.msg_size);
    int* eof_loc = (int*) &buf[0];
    *eof_loc = MQ_EOF_VAL;
    mq_send(mqd, buf, cli_args.msg_size, 0);

    free(buf);
    mq_close(mqd);
    free_word_count_array(pair_array);
    _exit(0);
}

mqd_t open_message_queue() {
    struct mq_attr mq_attrs = {
        .mq_msgsize = cli_args.msg_size,
        .mq_maxmsg = 10
    };
    mqd_t mqd = mq_open(MQ_NAME, O_RDWR | O_CREAT, 0660, &mq_attrs);
    if (mqd == (mqd_t) -1) {
        printf("Failed to open MQ: %d\n", errno);
        exit(6);
    }
    return mqd;
}

void spawn_child_processes(mqd_t mqd) {
    for (int i = 0; i != cli_args.in_file_count; i++) {
        pid_t child = fork();

        if (child == 0) {
            run_child_actions(i, mqd);
        } else if (child == -1) {
            printf("Failed to fork\n");
            exit(4);
        }
    }
}

struct word_count_array* await_child_processes(mqd_t mqd) {
    struct word_count_array* pair_array = init_word_count_array();

    char* buf = malloc(cli_args.msg_size);
    int completed_children_count = 0;
    while (completed_children_count < cli_args.in_file_count) {
        if (mq_receive(mqd, buf, cli_args.msg_size, NULL) == -1) {
            printf("Failed to receive message: %d\n", errno);
            exit(5);
        }
        print_buf(buf, cli_args.msg_size, "received:");

        int incoming_pairs_count = *(int*)&buf[0];
        if (incoming_pairs_count == MQ_EOF_VAL) {
            completed_children_count++;
            continue;
        }

        int buf_i = sizeof incoming_pairs_count;
        for (int i = 0; i != incoming_pairs_count; i++) {
            int word_len = strlen(&buf[buf_i]) + 1;
            char* word = malloc(word_len);
            strcpy(word, &buf[buf_i]);
            buf_i += word_len;

            int freq = *(int*)&buf[buf_i];
            buf_i += sizeof freq;

            increment_word(pair_array, word, freq);
            free(word);
        }
    }
    free(buf);

    mq_close(mqd);
    mq_unlink(MQ_NAME);

    return pair_array;
}

int main(int argc, char* argv[]) {
    start_time();

    cli_args = parse_arguments(argc, argv, 1);

    mqd_t mqd = open_message_queue();

    spawn_child_processes(mqd);

    struct word_count_array* pair_array = await_child_processes(mqd);

    write_output_file(pair_array, cli_args.out_file_name);
    free_word_count_array(pair_array);

    end_time();
    return 0;
}

