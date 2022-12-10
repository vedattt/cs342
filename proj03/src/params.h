#ifndef __GUARD_PARAMS_H
#define __GUARD_PARAMS_H

#include <stddef.h>

enum input_method { INPUT_METHOD_FILE, INPUT_METHOD_RANDOM };

enum replacement_algorithm { REPLACEMENT_LRU = 1, REPLACEMENT_FIFO = 2 };

struct virtual_regions {
    int base;
    int past_end;
    struct virtual_regions* next_region;
};

struct virtual_address {
    int address;
    struct virtual_address* next_address;
};

struct file_params {
    struct virtual_address* virtual_addresses;
};

struct param_store {
    enum input_method input_method;
    enum replacement_algorithm replacement_algorithm;
    int num_of_frames;
    int total_virtual_address_input_length;
    char* out_file_name;

    struct virtual_regions* virtual_regions;

    struct file_params* file_params;
};

struct param_virtual_address_iterator {
    struct param_store* param_store;
    struct virtual_address* next_virtual_address;
    int iteration_count;
};

struct param_store* parse_arguments(int argc, char* args[]);
void free_arguments(struct param_store* param_store);

struct param_virtual_address_iterator init_params_iterator(struct param_store* param_store);
int has_next_virtual_address(struct param_virtual_address_iterator* params_iterator);
struct virtual_address get_next_virtual_address(struct param_virtual_address_iterator* params_iterator);

#endif // !__GUARD_PARAMS_H

