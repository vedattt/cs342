#include "params.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"

void print_arguments(struct param_store* param_store) {
#ifdef DEBUG_LOG
    printf("input_method: %d (file: %d, random: %d)\n", param_store->input_method, INPUT_METHOD_FILE, INPUT_METHOD_RANDOM);

    printf("common params -> M: %d, out_file_name: %s, replacement_algorithm: %d, total_virt_addr_in_length: %d\n",
            param_store->num_of_frames,
            param_store->out_file_name,
            param_store->replacement_algorithm,
            param_store->total_virtual_address_input_length);

    if (param_store->file_params) {
        printf("file params ->\n");
        printf("\tvirtual addresses ->\n");
        struct virtual_address* virtual_address_node = param_store->file_params->virtual_addresses;
        while (virtual_address_node) {
            printf("\t\tvirtual_address: 0x%08x\n",
                    virtual_address_node->address);
            virtual_address_node = virtual_address_node->next_address;
        }
    }

    printf("virtual regions ->\n");
    struct virtual_regions* node = param_store->virtual_regions;
    while (node) {
        printf("\tbase: 0x%08x, past_end: 0x%08x\n",
                node->base, node->past_end);
        node = node->next_region;
    }
#endif /* DEBUG_LOG */
}

void parse_input_virtual_regions(struct param_store* param_store, char* fname) {
    FILE* fp = fopen(fname, "r");
    if (!fp) {
        printf("failed to open file '%s'\n", fname);
        exit(1);
    }
    
    struct virtual_regions dummy_node;
    struct virtual_regions* node = &dummy_node;
    char address_base[11], address_past_end[11];
    while (fscanf(fp, " %s %s", address_base, address_past_end) == 2) {
        node->next_region = malloc(sizeof *node->next_region);
        node->next_region->base = (int)strtol(address_base, NULL, 0);
        node->next_region->past_end = (int)strtol(address_past_end, NULL, 0);
        node->next_region->next_region = NULL;

        node = node->next_region;
    }
    fclose(fp);

    param_store->virtual_regions = dummy_node.next_region;
}

void parse_input_virtual_addresses(struct param_store* param_store, char* fname) {
    param_store->file_params = malloc(sizeof *param_store->file_params);

    FILE* fp = fopen(fname, "r");
    if (!fp) {
        printf("failed to open file '%s'\n", fname);
        exit(1);
    }
    
    int total_addresses = 0;
    struct virtual_address dummy_node;
    struct virtual_address* node = &dummy_node;
    char address[11];
    while (fscanf(fp, " %s", address) == 1) {
        node->next_address = malloc(sizeof *node->next_address);
        node->next_address->address = (int)strtol(address, NULL, 0);
        node->next_address->next_address = NULL;

        node = node->next_address;
        total_addresses++;
    }
    fclose(fp);

    param_store->file_params->virtual_addresses = dummy_node.next_address;
    param_store->total_virtual_address_input_length = total_addresses;
}

struct param_store* parse_arguments(int argc, char* args[]) {
    struct param_store* param_store = malloc(sizeof *param_store);
    param_store->file_params = NULL;

    int is_input_random = argc == 9;
    param_store->input_method = is_input_random ? INPUT_METHOD_RANDOM : INPUT_METHOD_FILE;
    
    char* output_file_name;

    if (is_input_random) {
        param_store->num_of_frames = atoi(args[1]);
        output_file_name = args[2];
        param_store->replacement_algorithm = atoi(args[4]);

        param_store->virtual_regions = malloc(sizeof *param_store->virtual_regions);
        param_store->virtual_regions->base = 0;
        param_store->virtual_regions->past_end = (int)strtol(args[6], NULL, 0);
        param_store->virtual_regions->next_region = NULL;

        param_store->total_virtual_address_input_length = atoi(args[8]);
    } else {
        parse_input_virtual_regions(param_store, args[1]);
        parse_input_virtual_addresses(param_store, args[2]);
        param_store->num_of_frames = atoi(args[3]);
        output_file_name = args[4];
        param_store->replacement_algorithm = atoi(args[6]);
    }

    param_store->out_file_name = malloc(strlen(output_file_name) + 1);
    strcpy(param_store->out_file_name, output_file_name);

    print_arguments(param_store);
    return param_store;
}

void free_arguments(struct param_store* param_store) {
    struct virtual_regions* region_node = param_store->virtual_regions;
    while (region_node) {
        struct virtual_regions* temp_node = region_node;
        region_node = region_node->next_region;
        free(temp_node);
    }

    if (param_store->file_params) {
        struct virtual_address* address_node = param_store->file_params->virtual_addresses;
        while (address_node) {
            struct virtual_address* temp_node = address_node;
            address_node = address_node->next_address;
            free(temp_node);
        }
        free(param_store->file_params);
    }

    free(param_store->out_file_name);
    free(param_store);
}

struct param_virtual_address_iterator init_params_iterator(struct param_store* param_store) {
    struct param_virtual_address_iterator iterator = {
        .param_store = param_store,
        .iteration_count = 0,
        .next_virtual_address = NULL
    };

    if (param_store->file_params)
        iterator.next_virtual_address = param_store->file_params->virtual_addresses;

    return iterator;
}

int has_next_virtual_address(struct param_virtual_address_iterator* params_iterator) {
    return (params_iterator->param_store->input_method == INPUT_METHOD_RANDOM
        && params_iterator->param_store->total_virtual_address_input_length > params_iterator->iteration_count)
        || (params_iterator->param_store->input_method == INPUT_METHOD_FILE
        && params_iterator->next_virtual_address);
}

struct virtual_address get_next_virtual_address(struct param_virtual_address_iterator* params_iterator) {
    struct virtual_address result;

    switch (params_iterator->param_store->input_method) {
        case INPUT_METHOD_RANDOM:
            {
                struct virtual_address random_address = {
                    .address = get_random_in_range(0, params_iterator->param_store->virtual_regions->past_end - 1)
                };
                result = random_address;
                break;
            }
        case INPUT_METHOD_FILE:
            {
                struct virtual_address* temp_node = params_iterator->next_virtual_address;
                params_iterator->next_virtual_address = params_iterator->next_virtual_address->next_address;
                result = *temp_node;
                break;
            }
    }
    params_iterator->iteration_count++;

#ifdef DEBUG_LOG
    static int process_index = 1;
    printf("iterator -> virtual_address #%d -> 0x%08x\n",
            process_index++,
            result.address);
#endif /* DEBUG_LOG */

    return result;
}

