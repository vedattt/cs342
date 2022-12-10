#include "utility.h"
#include "params.h"
#include "paging.h"
#include <stdio.h>
#include <stdlib.h>

void print_paging_results(struct param_store* param_store, struct paging_result_store result_store) {
    FILE* fp = fopen(param_store->out_file_name, "w");
    if (!fp) {
        printf("error: failed to open output file '%s'\n", param_store->out_file_name);
        exit(1);
    }

    for (int i = 0; i != param_store->total_virtual_address_input_length; i++) {
        struct paging_result* result = &result_store.results[i];

        if (param_store->input_method == INPUT_METHOD_RANDOM)
            fprintf(fp, "0x%08x ", result->virtual_address);

        if (result->unused_memory_access_exception)
            fprintf(fp, "0x%08x e", result->virtual_address);
        else
            fprintf(fp, "0x%08x", result->physical_address);

        if (result->caused_page_fault)
            fprintf(fp, " x");

        fprintf(fp, "\n");
    }
    fclose(fp);
}

int main(int argc, char* argv[]) {
    struct param_store* param_store = parse_arguments(argc, argv);

    struct paging_result_store results = run_paging_simulation(param_store);
    print_paging_results(param_store, results);
    
    free_paging_result_store(&results);

    free_arguments(param_store);
    return 0;
}

