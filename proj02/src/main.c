#include "generator.h"
#include "scheduler.h"
#include "workload_params.h"

struct workload_param_store* workload_params;

int main(int argc, char* argv[]) {
    workload_params = parse_arguments(argc, argv);

    pthread_t scheduler_tid = create_scheduler_thread(workload_params);
    pthread_t generator_tid = create_generator_thread(workload_params);

    pthread_join(scheduler_tid, NULL);
    pthread_join(generator_tid, NULL);

    free_arguments(workload_params);
    return 0;
}

