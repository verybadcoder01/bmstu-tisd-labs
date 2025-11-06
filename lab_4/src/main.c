#define _GNU_SOURCE
#include "stack_arr.h"
#include "stack_list.h"
#include "stdio.h"
#include "utils.h"
#include "string.h"
#include "perf_test.h"

typedef enum
{
    ARRAY = 1,
    LIST
} st_type;

typedef enum
{
    PUSH = 1,
    POP,
    PRINT,
    PRINT_REV,
    FINISH
} menu_item;

int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "p"))
    {
        error rc = run_perf_test();
        if (rc)
        {
            interface_printf_err(rc);
            return rc;
        }
        return 0;
    }
    stack_arr st_arr;
    st_arr.size = 0;
    stack_list st_list;
    st_list.head = NULL;
    st_list.size = 0;
    st_list.freed_cap = 0;
    st_list.freed_count = 0;
    st_list.freed_ptrs = NULL;

    st_type type;
    interface_printf("Введите тип стека: 1 - на массиве, 2 - на списке\n");
    if (scanf("%d", (int *)(&type)) != 1)
    {
        interface_printf_err(INPUT_ERROR);
        return INPUT_ERROR;
    }
    if (type != ARRAY && type != LIST)
    {
        interface_printf_err(WRONG_STACK_TYPE);
        return WRONG_STACK_TYPE;
    }
    menu_item item;
    while (1)
    {
        interface_printf("Выберите действие\n");
        interface_printf_menu();
        if (scanf("%d", (int *)(&item)) != 1)
        {
            interface_printf_err(INPUT_ERROR);
            delete_stack_list(&st_list);
            delete_stack_arr(&st_arr);
            return INPUT_ERROR;
        }
        switch (item)
        {
        case PUSH:
        {
            interface_printf("Введите элемент стека (слово)\n");
            char *data = NULL;
            size_t len = 0;
            clear_stdin();
            if (getline(&data, &len, stdin) == -1)
            {
                delete_stack_list(&st_list);
                delete_stack_arr(&st_arr);
                interface_printf_err(INPUT_ERROR);
                return INPUT_ERROR;
            }
            data[strcspn(data, "\n")] = '\0';
            error rc = 0;
            if (type == ARRAY)
            {
                rc = push_stack_arr(&st_arr, data);
            }
            else
            {
                rc = stack_list_push(&st_list, data);
            }
            free(data);
            if (rc && rc != STACK_OVERFLOW)
            {
                delete_stack_list(&st_list);
                delete_stack_arr(&st_arr);
                interface_printf_err(rc);
                return rc;
            }
            else if (rc == STACK_OVERFLOW)
            {
                interface_printf_err(rc);
            }
            break;
        }
        case POP:
        {
            error rc = 0;
            char *data = NULL;
            if (type == ARRAY)
            {
                rc = pop_stack_arr(&st_arr, &data);
            }
            else
            {
                rc = stack_list_pop(&st_list, &data);
            }
            if (rc && rc != EMPTY_STACK)
            {
                delete_stack_list(&st_list);
                delete_stack_arr(&st_arr);
                interface_printf_err(rc);
                return rc;
            }
            else if (rc == EMPTY_STACK)
            {
                interface_printf_err(rc);
            }
            if (!rc)
            {
                printf("%s\n", data);
                free(data);
            }
            break;
        }
        case PRINT:
        {
            if (type == ARRAY)
            {
                print_st_arr(&st_arr);
            }
            else
            {
                stack_list_print(&st_list);
            }
            break;
        }
        case PRINT_REV:
        {
            if (type == ARRAY)
            {
                print_rev_st_arr(&st_arr);
            }
            else
            {
                stack_list_print_rev(&st_list);
            }
            break;
        }
        case FINISH:
            goto finish;
        default:
            delete_stack_list(&st_list);
            delete_stack_arr(&st_arr);
            interface_printf_err(WRONG_MENU_ITEM);
            return WRONG_MENU_ITEM;
        }
    }
finish:
    delete_stack_list(&st_list);
    delete_stack_arr(&st_arr);
    return 0;
}
