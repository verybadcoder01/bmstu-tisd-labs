#ifndef SIMULATION_H__
#define SIMULATION_H__

#include "errors.h"
#include "queue_arr.h"
#include "request.h"
#include "time.h"
#include "queue_list.h"

typedef struct {
    double min, max;
} interval;

typedef struct {
    double total_queue_time1, total_queue_time2;
    double total_queue_length1, total_queue_length2;
    double idle_time;
    double last_event_time;
    int served1, served2;
    double current_time;
    double next_arrival1;
    double next_arrival2;
    double next_completion;
    request current_request;
    int is_serving;
    double total_push_time, total_pop_time;
    queue_list *ql1, *ql2;
    queue_arr *qa1, *qa2;
    queue_type used_type;
} simulation;

int validate_interval(interval ir);

double rand_val(interval ir);

size_t get_queue1_size(simulation *sim);

size_t get_queue2_size(simulation *sim);

error push_queue(simulation *sim, request *req, mode print_ptr, int queue_num);

error pop_queue(simulation *sim, request *req, mode add_history, int queue_num);

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end);

void print_core_data(simulation *sim);

void print_sim_data(simulation *sim);

error input_intervals(interval *gen_order1, interval *gen_order2, interval *process_order1, interval *process_order2);

error simulate(mode m, queue_type t);

#endif