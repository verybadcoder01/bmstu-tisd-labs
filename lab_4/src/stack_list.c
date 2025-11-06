#include "stack_list.h"
#include "string.h"
#include "utils.h"
#include "stdio.h"

error create_node(node **res, char *data)
{
    *res = malloc(sizeof(node));
    if (!*res)
    {
        return ALLOC_ERROR;
    }
    (*res)->data = malloc(strlen(data) + 1);
    if (!((*res)->data))
    {
        return ALLOC_ERROR;
    }
    strcpy((*res)->data, data);
    (*res)->next = NULL;
    return 0;
}

void delete_node(node **data)
{
    if (!*data)
    {
        return;
    }
    free((*data)->data);
    free(*data);
    *data = NULL;
}

error stack_list_push(stack_list *st, char *n_elem)
{
    if (st->size >= MAX_STACK_LIST_SIZE)
    {
        return STACK_OVERFLOW;
    }
    node *n_node = NULL;
    error rc = create_node(&n_node, n_elem);
    if (rc)
    {
        delete_node(&n_node);
        return rc;
    }
    interface_printf("Выделенный адрес:\n");
    interface_printf_ptr(n_node);
    n_node->next = st->head;
    st->head = n_node;
    st->size++;
    return 0;
}

error add_freed_ptr(stack_list *st, const node *ptr)
{
    if (st->freed_count < st->freed_cap)
    {
        st->freed_ptrs[st->freed_count] = ptr;
    }
    else
    {
        size_t n_cap = 0;
        if (st->freed_cap == 0)
        {
            n_cap = 1;
        }
        else
        {
            n_cap = 2 * st->freed_cap;
        }
        const node **tmp_buf = realloc(st->freed_ptrs, n_cap * sizeof(node *));
        if (!tmp_buf)
        {
            return ALLOC_ERROR;
        }
        st->freed_ptrs = tmp_buf;
        st->freed_cap = n_cap;
        st->freed_ptrs[st->freed_count] = ptr;
    }
    st->freed_count++;
    return 0;
}

error stack_list_pop(stack_list *st, char **res)
{
    if (st->size == 0)
    {
        return EMPTY_STACK;
    }
    node *temp = st->head;
    #ifdef DEBUG_PRINT
    error rc = add_freed_ptr(st, temp);
    if (rc)
    {
        return rc;
    }
    interface_printf("Очищенный адрес:\n");
    interface_printf_ptr(temp);
    #endif
    *res = temp->data;
    st->head = st->head->next;
    free(temp);
    st->size--;
    return 0;
}

void stack_list_print(stack_list *st)
{
    interface_printf("--- Вершина стека ---\n");
    for (node *cur = st->head; cur; cur = cur->next)
    {
        interface_printf("Значение:\n");
        printf("%s\n", cur->data);
        interface_printf("Адрес:\n");
        interface_printf_ptr(cur);
    }
    interface_printf("--- Дно стека ---\n");
    interface_printf("Очищенные адреса:\n");
    for (size_t i = 0; i < st->freed_count; ++i)
    {
        interface_printf_ptr(st->freed_ptrs[i]);
    }
}

void print_node_rev_recursive(node *n)
{
    if (n == NULL)
    {
        return;
    }
    print_node_rev_recursive(n->next);
    interface_printf("Значение:\n");
    size_t len = strlen(n->data);
    for (int i = len - 1; i >= 0; --i)
    {
        printf("%c", n->data[i]);
    }
    printf("\n");
    interface_printf("Адрес:\n");
    interface_printf_ptr(n);
}

void stack_list_print_rev(stack_list *st)
{
    interface_printf("--- Дно стека ---\n");
    print_node_rev_recursive(st->head);
    interface_printf("--- Вершина стека ---\n");
    interface_printf("Очищенные адреса:\n");
    for (size_t i = 0; i < st->freed_count; ++i)
    {
        interface_printf_ptr(st->freed_ptrs[i]);
    }
}

void delete_stack_list(stack_list *st)
{
    if (st == NULL)
    {
        return;
    }
    node *cur = st->head;
    while (cur)
    {
        node *next = cur->next;
        delete_node(&cur);
        cur = next;
    }
    if (st->freed_ptrs != NULL)
    {
        free(st->freed_ptrs);
        st->freed_ptrs = NULL;
    }
    st->head = NULL;
    st->size = 0;
}