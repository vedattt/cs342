#ifndef __GUARD_GENERATOR_H
#define __GUARD_GENERATOR_H

#include <pthread.h>
#include "workload_params.h"

pthread_t create_generator_thread(struct workload_param_store* param_store);
void* start_generator_thread(void*);

#endif // !__GUARD_GENERATOR_H
