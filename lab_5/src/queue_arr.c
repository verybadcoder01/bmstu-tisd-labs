#include "queue_arr.h"
#include "errors.h"
#include "request.h"

queue_arr empty_queue_arr() {
    queue_arr q = {{{0, 0, -1}}, 0, 0, 0};
    return q;
}

error queue_arr_push(queue_arr *qa, request *req) {
    if (qa->size == MAX_QUEUE_ARR_SIZE){
        return QUEUE_ARR_OVERFLOW;
    }
    qa->data[qa->right_ptr] = *req;
    qa->right_ptr++;
    qa->right_ptr %= MAX_QUEUE_ARR_SIZE;
    qa->size++;
    return 0;
}

error queue_arr_pop(queue_arr *qa, request *deleted) {
    if (qa->size == 0) {
        return EMPTY_QUEUE_DEL;
    }
    *deleted = qa->data[qa->left_ptr];
    qa->left_ptr++;
    qa->left_ptr %= MAX_QUEUE_ARR_SIZE;
    qa->size--;
    return 0;
}
