#include "queue_list.h"
#include "errors.h"
#include "request.h"
#include "stdio.h"
#include "stdlib.h"

int printed_count = 0;

queue_list empty_queue(freed_history *fr)
{
    queue_list q = { NULL, NULL, 0, fr };
    return q;
}

error create_node(request *req, node **res)
{
    *res = malloc(sizeof(node));
    if (!*res)
    {
        return ALLOC_ERROR;
    }
    (*res)->req = *req;
    (*res)->next = NULL;
    return 0;
}

error queue_list_push(queue_list *ql, request *req, mode print_ptr)
{
    if (ql->size == MAX_QUEUE_LIST_SIZE)
    {
        return QUEUE_LIST_OVERFLOW;
    }
    node *n_node;
    error rc = create_node(req, &n_node);
    if (rc)
    {
        return rc;
    }
    if (print_ptr == USER && printed_count < 50)
    {
        printed_count++;
        printf("Выделенный адрес: %p\n", (void *)n_node);
    }
    if (ql->size == 0)
    {
        ql->head = n_node;
        ql->tail = n_node;
        ql->head->next = n_node;
    }
    else
    {
        ql->tail->next = n_node;
        ql->tail = n_node;
    }
    ql->size++;
    return 0;
}

error add_node_hist(queue_list *ql, node *n_old)
{
    if (ql->fr->size == ql->fr->cap)
    {
        size_t n_cap = 0;
        if (ql->fr->cap == 0)
        {
            n_cap = 1;
        }
        else
        {
            n_cap = 2 * ql->fr->cap;
        }
        void **tmp_buf = realloc(ql->fr->ptrs, n_cap * sizeof(void *));
        if (!tmp_buf)
        {
            return ALLOC_ERROR;
        }
        ql->fr->ptrs = tmp_buf;
        ql->fr->cap = n_cap;
    }
    ql->fr->ptrs[ql->fr->size] = n_old;
    ql->fr->size++;
    return 0;
}

error queue_list_pop(queue_list *ql, request *deleted, mode add_history)
{
    if (ql->size == 0)
    {
        return EMPTY_QUEUE_DEL;
    }
    if (add_history == USER && printed_count < 50)
    {
        printf("Освобожденный адрес %p\n", (void *)ql->head);
        printed_count++;
        error rc = add_node_hist(ql, ql->head);
        if (rc)
        {
            return rc;
        }
    }
    deleted->arrival_time = ql->head->req.arrival_time;
    deleted->service_start_time = ql->head->req.service_start_time;
    deleted->type = ql->head->req.type;
    if (ql->size == 1)
    {
        free(ql->head);
        ql->head = NULL;
        ql->tail = NULL;
    }
    else
    {
        node *tmp = ql->head;
        ql->head = ql->head->next;
        free(tmp);
    }
    ql->size--;
    return 0;
}
