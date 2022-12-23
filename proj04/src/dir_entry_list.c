#include "dir_entry_list.h"
#include <stdlib.h>

struct dir_entry_node_list* dir_entry_list;

void init_dir_entry_list() {
    dir_entry_list = malloc(sizeof *dir_entry_list);
    dir_entry_list->head = NULL;
}

void free_dir_entry_list() {
    struct dir_entry_node* node = dir_entry_list->head;
    while (node) {
        struct dir_entry_node* next = node->next;
        free(node->value);
        free(node);
        node = next;
    }
    free(dir_entry_list);
}

void append_dir_entry_list(struct ext2_direntry_t* dir_entry) {
    struct dir_entry_node* node = malloc(sizeof *node);
    node->value = dir_entry;
    node->next = NULL;

    if (dir_entry_list->head == NULL) {
        dir_entry_list->head = node;
        return;
    }

    struct dir_entry_node* end_node = dir_entry_list->head;
    while (end_node->next) end_node = end_node->next;
    end_node->next = node;
}

struct dir_entry_node* get_head_dir_entry_list() {
    return dir_entry_list->head;
}

