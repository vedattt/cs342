#include "utility.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

void seed_rand() {
    static int first_run = 1;
    if (first_run) {
        first_run = 0;
        srand(time(NULL));
    }
}

int get_random_in_range(int range_start, int range_end) {
    seed_rand();
    if (range_start == range_end) return range_start;
    return range_start + rand() / (RAND_MAX / (range_end - range_start + 1) + 1);
}

double get_random_double() {
    seed_rand();
    return (double)rand() / (double)RAND_MAX;
}

