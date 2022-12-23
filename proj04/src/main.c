#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "blockgroup_descriptor.h"
#include "directory_entry.h"
#include "inode.h"
#include "superblock.h"
#include "dir_entry_list.h"

//#define DEBUG_LOG

#define BLOCK_SIZE 4096

struct ext2_superblock_t* superblock;
struct ext2_blockgroup_descriptor_t* bg_descriptor;
struct ext2_inode_t* root_directory_inode;

unsigned char buffer[BLOCK_SIZE];
int fd, bytes_read;

void buffer_init(char* devname) {
    fd = open(devname, O_RDONLY);
    if (fd < 0) {
        printf("error: cannot open disk file\n");
        exit(1);
    }
}

/**
 * Used for debugging
 */
void buffer_print() {
    printf("read %d bytes\n", bytes_read);
    for (int i = 0; i != bytes_read; i++) {
        printf("%02x ", buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
}

void buffer_read_block(int block_number) {
    off_t offset = BLOCK_SIZE * block_number;
    lseek(fd, offset, SEEK_SET);
    bytes_read = read(fd, buffer, BLOCK_SIZE);

    if (bytes_read < 0) {
        printf("error: could not read file\n");
        exit(2);
    } else if (bytes_read == 0) {
        printf("warning: EOF\n");
    }
}

void buffer_free() {
    close(fd);
}

struct ext2_inode_t* buffer_get_inode(int inode_index) {
    int inodes_per_block = BLOCK_SIZE / superblock->s_inode_size;
    int block_number = (inode_index - 1) / inodes_per_block;
    buffer_read_block(bg_descriptor->bg_inode_table + block_number);
    struct ext2_inode_t* partial_inode_table = (struct ext2_inode_t*)buffer;

    struct ext2_inode_t* inode = malloc(superblock->s_inode_size);
    memcpy(inode, &partial_inode_table[(inode_index - 1) % inodes_per_block], sizeof *inode);
    return inode;
}

void init_superblock() {
    buffer_read_block(0);

    superblock = malloc(sizeof *superblock);
    memcpy(superblock, &buffer[1024], sizeof *superblock);
}

void init_bg_descriptor() {
    // The block group descriptor table starts on the first block following the superblock
    buffer_read_block(1); 

    bg_descriptor = malloc(sizeof *bg_descriptor);
    memcpy(bg_descriptor, buffer, sizeof *bg_descriptor);
}

void print_superblock() {
    printf("inodes count: %d\n", superblock->s_inodes_count);
    printf("free inodes count: %d\n", superblock->s_free_inodes_count);
    printf("block size: %d\n", 1024 << superblock->s_log_block_size);
    printf("blocks count: %d\n", superblock->s_blocks_count);
    printf("free blocks count: %d\n", superblock->s_free_blocks_count);
    printf("first data block: %d\n", superblock->s_first_data_block);
    printf("blocks per group: %d\n", superblock->s_blocks_per_group);
    printf("inodes per group: %d\n", superblock->s_inodes_per_group);
    printf("mount count: %d\n", superblock->s_mnt_count);
    printf("max mount count: %d\n", superblock->s_max_mnt_count);
}

void print_inode(struct ext2_inode_t* inode) {
    printf("size: %d\n", inode->i_size);
    printf("uid: %d\n", inode->i_uid);
    printf("gid: %d\n", inode->i_gid);
    printf("links count: %d\n", inode->i_links_count);
    printf("blocks: %d\n", inode->i_blocks);
    printf("first block: %d\n", inode->i_block[0]);
    printf("generation: %d\n", inode->i_generation);
    printf("mode: %d\n", inode->i_mode);
    printf("flags: %d\n", inode->i_flags);

    time_t ctime = inode->i_ctime;
    struct tm* ctime_tm = gmtime(&ctime);
    printf("ctime: %02d:%02d:%02d on %d/%d\n",
        ctime_tm->tm_hour,
        ctime_tm->tm_min,
        ctime_tm->tm_sec,
        ctime_tm->tm_mday,
        ctime_tm->tm_mon);
}

void print_root_directory_entries() {
    struct dir_entry_node* node = get_head_dir_entry_list();

    while (node) {
        struct ext2_direntry_t* dir_entry = node->value;

        printf("%.*s\n", dir_entry->name_len, dir_entry->file_name);

#ifdef DEBUG_LOG
        printf("inode: %d, record length: %d, file type: %d\n",
            dir_entry->inode,
            dir_entry->rec_len,
            dir_entry->file_type);
#endif

        node = node->next;
    }
}

void print_root_directory_inodes() {
    struct dir_entry_node* node = get_head_dir_entry_list();

    while (node) {
        printf("---- inode for file '%.*s'\n", node->value->name_len, node->value->file_name);
        struct ext2_inode_t* inode = buffer_get_inode(node->value->inode);
        print_inode(inode);
        printf("\n");
        free(inode);

        node = node->next;
    }
}

void populate_dir_entry_list() {
    int block_offset = 0;
    buffer_read_block(root_directory_inode->i_block[block_offset]);

    int dir_entry_offset = 0;
    struct ext2_direntry_t* dir_entry = (struct ext2_direntry_t*)buffer;

    while (dir_entry->inode) {
        struct ext2_direntry_t* copy_dir_entry = malloc(sizeof *copy_dir_entry);
        memcpy(copy_dir_entry, dir_entry, sizeof *copy_dir_entry);
        append_dir_entry_list(copy_dir_entry);

        dir_entry_offset += dir_entry->rec_len;
        if (dir_entry_offset % BLOCK_SIZE == 0)
            buffer_read_block(block_offset++);

        dir_entry = (struct ext2_direntry_t*)&buffer[dir_entry_offset % BLOCK_SIZE];
    }
}

int main(int argc, char* argv[]) {
    buffer_init(argv[1]);
    init_superblock();
    init_bg_descriptor();
    init_dir_entry_list();
    root_directory_inode = buffer_get_inode(EXT2_ROOT_INO);

    printf("-- superblock info\n");
    print_superblock();
    printf("\n");

    populate_dir_entry_list();

    printf("-- root directory entries\n");
    print_root_directory_entries();
    printf("\n");

    printf("-- root directory inodes\n");
    print_root_directory_inodes();

    free(superblock);
    free(bg_descriptor);
    free(root_directory_inode);
    free_dir_entry_list();
    buffer_free();
    return 0;
}

