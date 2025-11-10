#ifndef QUEUE_ARR_H__
#define QUEUE_ARR_H__

#define MAX_QUEUE_ARR_SIZE 10000

#include "stddef.h"
#include "request.h"
#include "errors.h"

typedef struct {
    request data[MAX_QUEUE_ARR_SIZE];
    size_t left_ptr, right_ptr;
    size_t size;
} queue_arr;

queue_arr empty_queue_arr();

error queue_arr_push(queue_arr *qa, request *req);

error queue_arr_pop(queue_arr *qa, request *deleted);

#endif