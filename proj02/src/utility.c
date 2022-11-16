#include "utility.h"
#include <math.h>
#include <time.h>
#include <stdlib.h>

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

int get_exponential_random(int range_average, int range_start, int range_end) {
    seed_rand();

    double lambda = 1.0 / (double)range_average;

    double x;
    do {
        double u = get_random_double();
        x = -log(1 - u) / lambda;
    } while (x < range_start || x > range_end);
    return round(x);
}

