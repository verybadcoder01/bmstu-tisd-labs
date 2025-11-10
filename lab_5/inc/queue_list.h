#ifndef QUEUE_LIST_H__
#define QUEUE_LIST_H__

#include "errors.h"
#include "request.h"
#include "stddef.h"
#include "stdbool.h"

#define MAX_QUEUE_LIST_SIZE 10000

typedef struct node node;

struct node {
    request req;
    node *next;
};

typedef struct {
    void **ptrs;
    size_t size, cap;
} freed_history;

typedef struct {
    node *head, *tail;
    size_t size;
    freed_history *fr;
} queue_list;

queue_list empty_queue(freed_history *fr);

error add_node_hist(queue_list *ql, node *n_old);

error create_node(request *req, node **res);

error queue_list_push(queue_list *ql, request *req, mode print_ptr);

error queue_list_pop(queue_list *ql, request *deleted, mode add_history);

#endif