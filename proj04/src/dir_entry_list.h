#ifndef __GUARD_DIR_ENTRY_LIST_H
#define __GUARD_DIR_ENTRY_LIST_H

struct dir_entry_node {
    struct ext2_direntry_t* value;
    struct dir_entry_node* next;
};

struct dir_entry_node_list {
    struct dir_entry_node* head;
};

void init_dir_entry_list();
void free_dir_entry_list();
void append_dir_entry_list(struct ext2_direntry_t* dir_entry);
struct dir_entry_node* get_head_dir_entry_list();

#endif // !__GUARD_DIR_ENTRY_LIST_H

