#ifndef __GUARD_UTILITY_H
#define __GUARD_UTILITY_H

#include "process.h"

//#define DEBUG_LOG

int get_random_in_range(int range_start, int range_end);
int get_exponential_random(int range_average, int range_start, int range_end);

void start_simulation_time();
int get_time_since_t0();

long long start_time();
int end_time(long long time_start);

void init_process_stats_array(int max_pid);
void send_process_stats(struct process_statistics stats);
struct process_statistics* get_process_stats();

#endif // !__GUARD_UTILITY_H

