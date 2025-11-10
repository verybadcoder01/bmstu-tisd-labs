#include "simulation.h"
#include "errors.h"
#include "queue_arr.h"
#include "queue_list.h"
#include "request.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include <math.h>
#include <time.h>

int validate_interval(interval ir)
{
    if (ir.min < 0 || ir.max < 0 || ir.max < ir.min)
    {
        return 0;
    }
    return 1;
}

double rand_val(interval ir)
{
    return (double)rand() / (double)RAND_MAX * (ir.max - ir.min) + ir.min;
}

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) * (unsigned long)1e9 + (end->tv_nsec - start->tv_nsec);
}

size_t get_queue1_size(simulation *sim)
{
    if (sim->used_type == ARRAY)
    {
        return sim->qa1->size;
    }
    return sim->ql1->size;
}

size_t get_queue2_size(simulation *sim)
{
    if (sim->used_type == ARRAY)
    {
        return sim->qa2->size;
    }
    return sim->ql2->size;
}

error push_queue(simulation *sim, request *req, mode print_ptr, int queue_num)
{
    error rc = 0;
    if (sim->used_type == LIST)
    {
        if (queue_num == 1)
        {
            rc = queue_list_push(sim->ql1, req, print_ptr);
        }
        else
        {
            rc = queue_list_push(sim->ql2, req, print_ptr);
        }
    }
    else if (sim->used_type == ARRAY)
    {
        if (queue_num == 1)
        {
            rc = queue_arr_push(sim->qa1, req);
        }
        else
        {
            rc = queue_arr_push(sim->qa2, req);
        }
    }
    return rc;
}

error pop_queue(simulation *sim, request *req, mode add_history, int queue_num)
{
    error rc = 0;
    if (sim->used_type == LIST)
    {
        if (queue_num == 1)
        {
            rc = queue_list_pop(sim->ql1, req, add_history);
        }
        else
        {
            rc = queue_list_pop(sim->ql2, req, add_history);
        }
    }
    else if (sim->used_type == ARRAY)
    {
        if (queue_num == 1)
        {
            rc = queue_arr_pop(sim->qa1, req);
        }
        else
        {
            rc = queue_arr_pop(sim->qa2, req);
        }
    }
    return rc;
}

error input_intervals(interval *gen_order1, interval *gen_order2, interval *process_order1, interval *process_order2)
{
    printf("Введите интервал поступления заявки 1го типа. По варианту - от 1 до 5 в.е.\n");
    if (scanf("%lf %lf", &gen_order1->min, &gen_order1->max) != 2)
    {
        return INPUT_ERROR;
    }
    if (!validate_interval(*gen_order1))
    {
        return WRONG_TIME;
    }
    printf("Введите интервал поступления заявки 2го типа. По варианту - от 0 до 3 в.е.\n");
    if (scanf("%lf %lf", &gen_order2->min, &gen_order2->max) != 2)
    {
        return INPUT_ERROR;
    }
    if (!validate_interval(*gen_order2))
    {
        return WRONG_TIME;
    }
    printf("Введите интервал обработки заявки 1го типа. По варианту - от 0 до 4 в.е.\n");
    if (scanf("%lf %lf", &process_order1->min, &process_order1->max) != 2)
    {
        return INPUT_ERROR;
    }
    if (!validate_interval(*process_order1))
    {
        return WRONG_TIME;
    }
    printf("Введите интервал обработки заявки 2го типа. По варианту - от 0 до 1 в.е.\n");
    if (scanf("%lf %lf", &process_order2->min, &process_order2->max) != 2)
    {
        return INPUT_ERROR;
    }
    if (!validate_interval(*process_order2))
    {
        return WRONG_TIME;
    }
    return 0;
}

error start_serving_next_request(simulation *sim, interval T3, interval T4, mode m)
{
    struct timespec start_time, end_time;
    request next_request;
    error rc = 0;
    if (get_queue1_size(sim) > 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        rc = pop_queue(sim, &next_request, m, 1);
        clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
    else if (get_queue2_size(sim) > 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        rc = pop_queue(sim, &next_request, m, 2);
        clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
    else
    {
        return 0;
    }
    if (rc)
    {
        return rc;
    }
    sim->total_pop_time += elapsed_time(&start_time, &end_time);
    next_request.service_start_time = sim->current_time;

    double queue_time = next_request.service_start_time - next_request.arrival_time;
    if (next_request.type == PRIVILEGED)
    {
        sim->total_queue_time1 += queue_time;
    }
    else
    {
        sim->total_queue_time2 += queue_time;
    }

    double service_time = (next_request.type == PRIVILEGED) ? rand_val(T3) : rand_val(T4);
    sim->next_completion = sim->current_time + service_time;
    sim->is_serving = 1;

    sim->current_request = next_request;
    return 0;
}

void print_core_data(simulation *sim)
{
    double avg_queue_time1 = sim->total_queue_time1 / sim->served1;
    double avg_queue_time2 = (sim->served2 > 0) ? sim->total_queue_time2 / sim->served2 : 0.0;
    double avg_queue_length1 = sim->total_queue_length1 / sim->current_time;
    double avg_queue_length2 = sim->total_queue_length2 / sim->current_time;
    printf("Обслужено заявок 1-го типа: %d\n", sim->served1);
    printf("Обслужено заявок 2-го типа: %d\n", sim->served2);
    printf("Осталось в очереди заявок 1-го типа: %zu\n", get_queue1_size(sim));
    printf("Осталось в очереди заявок 2-го типа: %zu\n", get_queue2_size(sim));
    printf("Среднее время в очереди для 1-го типа: %.6f\n", avg_queue_time1);
    printf("Среднее время в очереди для 2-го типа: %.6f\n", avg_queue_time2);
    printf("Средняя длина очереди 1-го типа: %.6f\n", avg_queue_length1);
    printf("Средняя длина очереди 2-го типа: %.6f\n", avg_queue_length2);
}

void print_sim_data(simulation *sim)
{
    double avg_queue_time1 = sim->total_queue_time1 / sim->served1;
    double avg_queue_time2 = (sim->served2 > 0) ? sim->total_queue_time2 / sim->served2 : 0.0;
    double avg_queue_length1 = sim->total_queue_length1 / sim->current_time;
    double avg_queue_length2 = sim->total_queue_length2 / sim->current_time;
    printf("\n=== РЕЗУЛЬТАТЫ СИМУЛЯЦИИ ===\n");
    print_core_data(sim);
    printf("Суммарное время простоя аппарата: %.6f\n", sim->idle_time);
    printf("Общее время симуляции: %.6f\n", sim->current_time);

    printf("\n=== ПРОВЕРКА ===\n");
    printf("λ1 * W1 = %.6f, L1 = %.6f\n",
           (sim->served1 / sim->current_time) * avg_queue_time1, avg_queue_length1);
    printf("λ2 * W2 = %.6f, L2 = %.6f\n",
           (sim->served2 / sim->current_time) * avg_queue_time2, avg_queue_length2);

    printf("\nВремя операций с очередью:\n");
    printf("Среднее время push операций: %.10f нс\n", (double)sim->total_push_time / (double)(sim->served1 + get_queue1_size(sim) + sim->served2 + get_queue2_size(sim)));
    printf("Суммарное время pop операций: %.10f нс\n", (double)sim->total_pop_time / (double)(sim->served1 + sim->served2));
}

int select_next_event(simulation *sim)
{
    double next_event_time = INFINITY;
    int event_type = -1;
    if (sim->next_arrival1 < next_event_time)
    {
        next_event_time = sim->next_arrival1;
        event_type = 0;
    }
    if (sim->next_arrival2 < next_event_time)
    {
        next_event_time = sim->next_arrival2;
        event_type = 1;
    }
    if (sim->is_serving && sim->next_completion < next_event_time)
    {
        next_event_time = sim->next_completion;
        event_type = 2;
    }
    sim->current_time = next_event_time;
    return event_type;
}

error simulate(mode m, queue_type t)
{
    interval gen_order1, gen_order2;
    interval process_order1, process_order2;
    error rc = input_intervals(&gen_order1, &gen_order2, &process_order1, &process_order2);
    if (rc)
    {
        return rc;
    }
    srand((unsigned)time(NULL));

    freed_history fr1 = { NULL, 0, 0 }, fr2 = { NULL, 0, 0 };
    queue_list ql1 = empty_queue(&fr1), ql2 = empty_queue(&fr2);
    queue_arr qa1 = empty_queue_arr(), qa2 = empty_queue_arr();
    simulation sim = { 0, 0, 0, 0, 0, 0, 0, 0, 0, rand_val(gen_order1), rand_val(gen_order2), -1, { 0, 0, -1 }, 0, 0, 0, &ql1, &ql2, &qa1, &qa2, t };

    struct timespec start_time, end_time;

    printf("Начинаем симуляцию...\n");
    sim.idle_time += sim.next_arrival1 < sim.next_arrival2 ? sim.next_arrival1 : sim.next_arrival2;
    while (sim.served1 < 1000)
    {
        if (sim.current_time > sim.last_event_time)
        {
            double time_diff = sim.current_time - sim.last_event_time;
            sim.total_queue_length1 += get_queue1_size(&sim) * time_diff;
            sim.total_queue_length2 += get_queue2_size(&sim) * time_diff;
            if (!sim.is_serving)
            {
                sim.idle_time += time_diff;
            }
            sim.last_event_time = sim.current_time;
        }
        int event_type = select_next_event(&sim);

        switch (event_type)
        {
        case 0: {
            request new_request = { sim.current_time, -1, 1 };
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            rc = push_queue(&sim, &new_request, m, 1);
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            if (rc)
            {
                return rc;
            }
            sim.total_push_time += elapsed_time(&start_time, &end_time);
            sim.next_arrival1 = sim.current_time + rand_val(gen_order1);
            if (!sim.is_serving)
            {
                rc = start_serving_next_request(&sim, process_order1, process_order2, m);
            }
            if (rc)
            {
                return rc;
            }
            break;
        }

        case 1: {
            request new_request = { sim.current_time, -1, 2 };
            clock_gettime(CLOCK_MONOTONIC, &start_time);
            rc = push_queue(&sim, &new_request, m, 2);
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            if (rc)
            {
                return rc;
            }
            sim.total_push_time += elapsed_time(&start_time, &end_time);
            sim.next_arrival2 = sim.current_time + rand_val(gen_order2);
            if (!sim.is_serving && get_queue1_size(&sim))
            {
                rc = start_serving_next_request(&sim, process_order1, process_order2, m);
            }
            if (rc)
            {
                return rc;
            }
            break;
        }

        case 2: {
            if (sim.current_request.type == PRIVILEGED)
            {
                sim.served1++;
                if (sim.served1 % 100 == 0)
                {
                    print_core_data(&sim);
                }
            }
            else if (sim.current_request.type == REGULAR)
            {
                sim.served2++;
            }

            sim.is_serving = 0;
            sim.next_completion = -1.0;

            if (get_queue1_size(&sim) > 0 || get_queue2_size(&sim) > 0)
            {
                rc = start_serving_next_request(&sim, process_order1, process_order2, m);
            }
            if (rc)
            {
                return rc;
            }
            break;
        }
        }
    }
    print_sim_data(&sim);
    return 0;
}