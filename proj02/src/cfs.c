#include "cfs.h"
#include <math.h>
#include <stdlib.h>

int get_weight(int priority) {
    // From the Linux kernel
    const int prio_to_weight[40] = {
        /* -20 */     88761,     71755,     56483,     46273,     36291,
        /* -15 */     29154,     23254,     18705,     14949,     11916,
        /* -10 */      9548,      7620,      6100,      4904,      3906,
        /*  -5 */      3121,      2501,      1991,      1586,      1277,
        /*   0 */      1024,       820,       655,       526,       423,
        /*   5 */       335,       272,       215,       172,       137,
        /*  10 */       110,        87,        70,        56,        45,
        /*  15 */        36,        29,        23,        18,        15,
    };
    return prio_to_weight[priority + 20];
}

struct runqueue* runqueue_init() {
    struct runqueue* rq = malloc(sizeof *rq);
    rq->head = NULL;
    rq->count = 0;
    return rq;
}

void runqueue_free(struct runqueue* rq) {
    free(rq);
}

void runqueue_add_pcb(struct runqueue* rq, struct process_control_block* pcb) {
    struct runqueue_node* node = malloc(sizeof *node);
    node->pcb = pcb;
    node->next = rq->head;
    rq->head = node;
}

void runqueue_remove_pcb(struct runqueue* rq, struct process_control_block* pcb) {
    struct runqueue_node* prev_node = NULL;
    struct runqueue_node* node = rq->head;
    while (node) {
        if (node->pcb == pcb) {
            if (prev_node)
                prev_node->next = node->next;
            else
                rq->head = node->next;
            free(node);
            return;
        }
        prev_node = node;
        node = node->next;
    }
}

int runqueue_timeslice(struct runqueue* rq, int priority) {
    int weight = get_weight(priority);
    int total_weight = 0;
    
    struct runqueue_node* node = rq->head;
    while (node) {
        total_weight += get_weight(node->pcb->priority);
        node = node->next;
    }

    int timeslice = round(((double)weight / (double)total_weight) * SCHED_LATENCY);
    return timeslice < MIN_GRANULARITY ? MIN_GRANULARITY : timeslice;
}

double runqueue_vruntime(struct runqueue* rq, int priority, int actual_runtime) {
    return ((double)get_weight(0) / (double)get_weight(priority)) * actual_runtime;
}

struct process_control_block* runqueue_smallest_pcb(struct runqueue* rq) {
    if (rq->head == NULL) return NULL;

    struct runqueue_node* min_node = rq->head;
    struct runqueue_node* node = rq->head;

    while (node) {
        if (min_node->pcb->vruntime > node->pcb->vruntime)
            min_node = node;
        node = node->next;
    }

    return min_node->pcb;
}

