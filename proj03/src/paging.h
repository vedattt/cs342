#ifndef __GUARD_PAGING_H
#define __GUARD_PAGING_H

#include "params.h"

#define PAGE_TABLE_LENGTH 1024 // 2^10
#define MAX_FRAME_COUNT 1000

#define OUTER_INDEX_MASK 0xffc00000
#define INNER_INDEX_MASK 0x003ff000
#define PAGE_OFFSET_MASK 0x00000fff

struct logical_address {
    int outer_index;
    int inner_index;
    int page_offset;
};

struct page {
    int frame;
    int valid;

    int page_in_time;
    int last_used_time;
};

struct outer_page {
    struct page* inner_pages;
};

struct memory_frame {
    int taken;
};

struct paging_result {
    int virtual_address;
    int physical_address;
    int replaced_frame_index;
    int caused_page_fault;
    int unused_memory_access_exception;
};

struct paging_result_store {
    struct paging_result* results;
};

struct paging_result_store run_paging_simulation(struct param_store* param_store);

void free_paging_result_store(struct paging_result_store* store);

#endif // !__GUARD_PAGING_H

