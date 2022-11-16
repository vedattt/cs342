#include "workload_params.h"

struct workload_param_store* workload_params;

int main(int argc, char* argv[]) {
    workload_params = parse_arguments(argc, argv);
    return 0;
}

