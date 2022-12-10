#include "paging.h"
#include "params.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

struct logical_address get_logical_address(int virt_addr) {
    return (struct logical_address){
        .outer_index = (virt_addr & OUTER_INDEX_MASK) >> 22,
        .inner_index = (virt_addr & INNER_INDEX_MASK) >> 12,
        .page_offset = (virt_addr & PAGE_OFFSET_MASK)
    };
}

int is_virtual_address_in_range(struct param_store* param_store, int virtual_address) {
    struct virtual_regions* node = param_store->virtual_regions;
    while (node) {
        if (node->base <= virtual_address && virtual_address < node->past_end)
            return 1;
        node = node->next_region;
    }
    return 0;
}

int get_physical_address(int frame_index, int frame_offset) {
    return (frame_index << 12) + frame_offset;
}

int find_first_free_frame(struct param_store* param_store, struct memory_frame* frames) {
    for (int i = 0; i != param_store->num_of_frames; i++) {
        if (!frames[i].taken)
            return i;
    }
    return -1;
}

struct page* get_fifo_page(struct outer_page* outer_table) {
    int min_page_in_time = 999999;
    struct page* min_page;
    for (int i = 0; i != PAGE_TABLE_LENGTH; i++) {
        for (int j = 0; j != PAGE_TABLE_LENGTH; j++) {
            struct page* page = &outer_table[i].inner_pages[j];
            if (min_page_in_time > page->page_in_time && page->valid) {
                min_page_in_time = page->page_in_time;
                min_page = page;
            }
        }
    }
    return min_page;
}

struct page* get_lru_page(struct outer_page* outer_table) {
    int min_last_used_time = 999999;
    struct page* min_page;
    for (int i = 0; i != PAGE_TABLE_LENGTH; i++) {
        for (int j = 0; j != PAGE_TABLE_LENGTH; j++) {
            struct page* page = &outer_table[i].inner_pages[j];
            if (min_last_used_time > page->last_used_time && page->valid) {
                min_last_used_time = page->last_used_time;
                min_page = page;
            }
        }
    }
    return min_page;
}

struct page* get_victim_page(struct param_store* param_store, struct outer_page* outer_table) {
    struct page* replacement_fifo = get_fifo_page(outer_table);
    struct page* replacement_lru = get_lru_page(outer_table);

#ifdef DEBUG_LOG
    printf("frames -> replacement_fifo: %d, replacement_lru: %d\n",
        replacement_fifo->frame,
        replacement_lru->frame);
#endif

    switch (param_store->replacement_algorithm) {
        case REPLACEMENT_FIFO: return replacement_fifo;
        case REPLACEMENT_LRU: return replacement_lru;
        default:
            printf("error: unknown replacement algorithm given\n");
            exit(1);
    }
}

struct outer_page* init_outer_page_table() {
    struct outer_page* outer_table = malloc(sizeof *outer_table * PAGE_TABLE_LENGTH);
    for (int i = 0; i != PAGE_TABLE_LENGTH; i++) {
        outer_table[i].inner_pages = malloc(sizeof *outer_table[i].inner_pages * PAGE_TABLE_LENGTH);
        for (int j = 0; j != PAGE_TABLE_LENGTH; j++) {
            outer_table[i].inner_pages[j].frame = -1;
            outer_table[i].inner_pages[j].valid = 0;
            outer_table[i].inner_pages[j].page_in_time = -1;
            outer_table[i].inner_pages[j].last_used_time = -1;
        }
    }
    return outer_table;
}

void free_outer_page_table(struct outer_page* outer_table) {
    for (int i = 0; i != PAGE_TABLE_LENGTH; i++)
        free(outer_table[i].inner_pages);
    free(outer_table);
}

struct paging_result_store init_paging_result_store(struct param_store* param_store) {
    struct paging_result_store result_store = {
        .results = malloc(sizeof *result_store.results * param_store->total_virtual_address_input_length)
    };

    for (int i = 0; i != param_store->total_virtual_address_input_length; i++) {
        result_store.results[i].replaced_frame_index = -1;
        result_store.results[i].caused_page_fault = 0;
        result_store.results[i].unused_memory_access_exception = 0;
    }

    return result_store;
}

void free_paging_result_store(struct paging_result_store* store) {
    free(store->results);
}

struct memory_frame* init_frame_table() {
    struct memory_frame* frames = malloc(sizeof *frames * MAX_FRAME_COUNT);
    for (int i = 0; i != MAX_FRAME_COUNT; i++) {
        frames[i].taken = 0;
    }
    return frames;
}

struct paging_result_store run_paging_simulation(struct param_store *param_store) {
    struct paging_result_store result_store = init_paging_result_store(param_store);

    struct memory_frame* frames = init_frame_table();
    struct outer_page* outer_table = init_outer_page_table();

    int t = 0;
    struct param_virtual_address_iterator iter = init_params_iterator(param_store);
    while (has_next_virtual_address(&iter)) {
        struct paging_result* current_result = &result_store.results[t++];

        struct virtual_address virtual_address = get_next_virtual_address(&iter);
        current_result->virtual_address = virtual_address.address;

        if (!is_virtual_address_in_range(param_store, virtual_address.address)) {
            current_result->unused_memory_access_exception = 1;
            continue;
        }

        struct logical_address logical_address = get_logical_address(virtual_address.address);

        struct page* page = &outer_table[logical_address.outer_index].inner_pages[logical_address.inner_index];
        if (!page->valid) {
            current_result->caused_page_fault = 1;

            int free_frame_index = find_first_free_frame(param_store, frames);
            if (free_frame_index == -1) {
                struct page* victim_page = get_victim_page(param_store, outer_table);
                victim_page->valid = 0;
                free_frame_index = victim_page->frame;
#ifdef DEBUG_LOG
                printf("no free frames, replacing %d\n", free_frame_index);
#endif
            }

            frames[free_frame_index].taken = 1;
            page->frame = free_frame_index;
            page->valid = 1;
            page->page_in_time = t;

            current_result->replaced_frame_index = page->frame;
        }

        page->last_used_time = t;

        current_result->physical_address = get_physical_address(page->frame, logical_address.page_offset);
    }

    free(frames);
    free_outer_page_table(outer_table);

    return result_store;
}

