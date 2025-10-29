#include "stack_arr.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "utils.h"

error push_stack_arr(stack_arr *st, char *n_elem)
{
    if (st->size < MAX_ARR_STACK_SIZE)
    {
        st->data[st->size] = malloc(strlen(n_elem) + 1);
        if (!(st->data[st->size]))
        {
            return ALLOC_ERROR;
        }
        strcpy(st->data[st->size], n_elem);
        st->size++;
        return 0;
    }
    return STACK_OVERFLOW;
}

error pop_stack_arr(stack_arr *st)
{
    if (st->size == 0)
    {
        return EMPTY_STACK;
    }
    free(st->data[st->size - 1]);
    st->size--;
    return 0;
}

void print_st_arr(stack_arr *st){
    interface_printf("--- Вершина стека ---\n");
    for (int i = st->size - 1; i >= 0; --i){
        printf("%s\n", st->data[i]);
    }
    interface_printf("--- Дно стека ---\n");
}

void print_rev_st_arr(stack_arr *st){
    interface_printf("--- Дно стека ---\n");
    for (int i = 0; i < (int)st->size; ++i){
        size_t len = strlen(st->data[i]);
        for (int j = len - 1; j >= 0; --j){
            printf("%c", st->data[i][j]);
        }
        printf("\n");
    }
    interface_printf("--- Вершина стека ---\n");
}

void delete_stack_arr(stack_arr *st){
    for (size_t i = 0; i < st->size; ++i){
        free(st->data[i]);
    }
}