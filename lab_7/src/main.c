#define _GNU_SOURCE
#include "errors.h"
#include "hash_table.h"
#include "node.h"
#include "string.h"
#include "tree.h"
#include "utils.h"
#include <stdio.h>
#include "perf_test.h"

error handle_tree()
{
    node_t *root = NULL;
    tree_action ta = -1;
    while (ta != T_QUIT)
    {
        printf("Введите действие\n");
        print_menu_tree();
        if (scanf("%d", (int *)&ta) != 1)
        {
            printf("Ошибка %d\n", INPUT_ERROR);
            tree_delete(root);
            return INPUT_ERROR;
        }
        scan_newlines(stdin);
        switch (ta)
        {
        case T_BUILD: {
            FILE *fin = NULL;
            error rc = read_and_open_file(&fin, "r");
            if (rc)
            {
                continue;
            }
            rc = add_nums_from_file_to_tree(&root, fin);
            if (rc)
            {
                printf("Ошибка %d\n", rc);
                tree_delete(root);
                fclose(fin);
                return rc;
            }
            fclose(fin);
            break;
        }
        case T_ADD: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            error rc = tree_insert(&root, comp_node_data, num);
            if (rc)
            {
                if (rc == DUPLICATE_KEY)
                {
                    printf("Такой ключ уже существует в дереве!\n");
                    break;
                }
                else
                {
                    printf("Ошибка %d\n", rc);
                    tree_delete(root);
                    return rc;
                }
            }
            printf("Успешно\n");
            break;
        }
        case T_REMOVE: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            error rc = tree_remove(&root, comp_node_data, &num);
            if (rc)
            {
                if (rc == KEY_NOT_FOUND)
                {
                    printf("Такого ключа в дереве нет!\n");
                    break;
                }
                else
                {
                    printf("Ошибка %d\n", rc);
                    tree_delete(root);
                    return rc;
                }
            }
            printf("Успешно\n");
            break;
        }
        case T_FIND: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            int cmp_count = 0;
            error rc = tree_find(root, comp_node_data, &num, &cmp_count);
            if (rc)
            {
                printf("Ключ не найден!\n");
            }
            else
            {
                printf("Ключ найден! Потребовалось %d сравнений\n", cmp_count);
            }
            break;
        }
        case T_GRAPHVIZ: {
            FILE *fout = NULL;
            error rc = read_and_open_file(&fout, "w");
            if (rc)
            {
                continue;
            }
            serialize_to_graphviz(root, fout);
            fclose(fout);
            break;
        }
        case T_WALK: {
            printf("Префиксный обход (pre-order):\n");
            tree_walk(root, PREFIX);
            printf("Инфиксный обход (in-order):\n");
            tree_walk(root, INFIX);
            printf("Постфиксный обход (post-order):\n");
            tree_walk(root, POSTFIX);
            break;
        }
        case T_QUIT: {
            break;
        }
        default:
            printf("Ошибка %d\n", WRONG_TREE_ACTION);
        }
    }
    tree_delete(root);
    return 0;
}

error handle_ht_open()
{
    hash_table_open ht_open;
    table_action ta = -1;
    while (ta != H_QUIT)
    {
        printf("Введите действие\n");
        print_menu_table();
        if (scanf("%d", (int *)&ta) != 1)
        {
            printf("Ошибка %d\n", INPUT_ERROR);
            ht_open_delete(&ht_open);
            return INPUT_ERROR;
        }
        scan_newlines(stdin);
        switch (ta)
        {
        case H_BUILD: {
            FILE *fin = NULL;
            error rc = read_and_open_file(&fin, "r");
            if (rc)
            {
                continue;
            }
            int n_count = count_nums_in_file(fin);
            if (n_count == -1)
            {
                fclose(fin);
                printf("Ошибка %d\n", WRONG_FILE_CONTENT);
                continue;
            }
            rc = ht_open_init(&ht_open, n_count, MAX_OPEN_LOAD_FACTOR, hash_knuth_xor, second_hash_func, second_hash_probe);
            if (rc)
            {
                printf("Ошибка %d\n", rc);
                ht_open_delete(&ht_open);
                fclose(fin);
                return rc;
            }
            rc = add_nums_from_file_to_ht_open(&ht_open, fin);
            if (rc)
            {
                printf("Ошибка %d\n", rc);
                ht_open_delete(&ht_open);
                fclose(fin);
                return rc;
            }
            fclose(fin);
            break;
        }
        case H_ADD: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            error rc = ht_open_insert(&ht_open, num);
            if (rc)
            {
                if (rc == DUPLICATE_KEY)
                {
                    printf("Такой ключ уже существует!\n");
                    break;
                }
                else
                {
                    printf("Ошибка %d\n", rc);
                    ht_open_delete(&ht_open);
                    return rc;
                }
            }
            break;
        }
        case H_REMOVE: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            error rc = ht_open_remove(&ht_open, num);
            if (rc)
            {
                if (rc == KEY_NOT_FOUND)
                {
                    printf("Ключ не найден!\n");
                    break;
                }
                else
                {
                    printf("Ошибка %d\n", rc);
                    ht_open_delete(&ht_open);
                    return rc;
                }
            }
            break;
        }
        case H_FIND: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            int cmp_count = 0;
            error rc = ht_open_find(&ht_open, num, &cmp_count);
            if (rc)
            {
                printf("Ключ не найден!\n");
            }
            else
            {
                printf("Ключ найден! Потребовалось %d сравнений\n", cmp_count);
            }
            break;
        }
        case H_PRINT: {
            ht_open_print(&ht_open);
            break;
        }
        case H_QUIT: {
            break;
        }
        }
    }
    ht_open_delete(&ht_open);
    return 0;
}

error handle_ht_chain()
{
    hash_table_chain ht_chain;
    table_action ta = -1;
    while (ta != H_QUIT)
    {
        printf("Введите действие\n");
        print_menu_table();
        if (scanf("%d", (int *)&ta) != 1)
        {
            printf("Ошибка %d\n", INPUT_ERROR);
            ht_chain_delete(&ht_chain);
            return INPUT_ERROR;
        }
        scan_newlines(stdin);
        switch (ta)
        {
        case H_BUILD: {
            FILE *fin = NULL;
            error rc = read_and_open_file(&fin, "r");
            if (rc)
            {
                continue;
            }
            int n_count = count_nums_in_file(fin);
            if (n_count == -1)
            {
                fclose(fin);
                printf("Ошибка %d\n", WRONG_FILE_CONTENT);
                continue;
            }
            rc = ht_chain_init(&ht_chain, n_count, 0.7, hash_knuth_xor);
            if (rc)
            {
                printf("Ошибка %d\n", rc);
                ht_chain_delete(&ht_chain);
                fclose(fin);
                return rc;
            }
            rc = add_nums_from_file_to_ht_chain(&ht_chain, fin);
            if (rc)
            {
                printf("Ошибка %d\n", rc);
                ht_chain_delete(&ht_chain);
                fclose(fin);
                return rc;
            }
            fclose(fin);
            break;
        }
        case H_ADD: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            error rc = ht_chain_insert(&ht_chain, num);
            if (rc)
            {
                if (rc == DUPLICATE_KEY)
                {
                    printf("Такой ключ уже существует!\n");
                    break;
                }
                else
                {
                    printf("Ошибка %d\n", rc);
                    ht_chain_delete(&ht_chain);
                    return rc;
                }
            }
            break;
        }
        case H_REMOVE: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            error rc = ht_chain_remove(&ht_chain, num);
            if (rc)
            {
                if (rc == KEY_NOT_FOUND)
                {
                    printf("Ключ не найден!\n");
                    break;
                }
                else
                {
                    printf("Ошибка %d\n", rc);
                    ht_chain_delete(&ht_chain);
                    return rc;
                }
            }
            break;
        }
        case H_FIND: {
            int num = 0;
            if (scanf("%d", &num) != 1)
            {
                printf("Ошибка %d\n", INPUT_ERROR);
                continue;
            }
            scan_newlines(stdin);
            int cmp_count = 0;
            error rc = ht_chain_find(&ht_chain, num, &cmp_count);
            if (rc)
            {
                printf("Ключ не найден!\n");
            }
            else
            {
                printf("Ключ найден! Потребовалось %d сравнений\n", cmp_count);
            }
            break;
        }
        case H_PRINT: {
            ht_chain_print(&ht_chain);
            break;
        }
        case H_QUIT: {
            break;
        }
        }
    }
    ht_chain_delete(&ht_chain);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "p")){
        error rc = run_perf_test();
        if (rc){
            printf("Ошибка %d\n", rc);
        }
        return rc;
    }
    print_menu_struct();
    struct_type st;
    if (scanf("%d", (int *)(&st)) != 1)
    {
        return INPUT_ERROR;
    }
    if (st != AVL_TREE && st != HASH_TABLE_OPEN && st != HASH_TABLE_CHAIN)
    {
        return WRONG_STRUCT_TYPE;
    }
    error rc = 0;
    if (st == AVL_TREE)
    {
        rc = handle_tree();
    }
    else if (st == HASH_TABLE_CHAIN)
    {
        rc = handle_ht_chain();
    }
    else
    {
        rc = handle_ht_open();
    }
    return rc;
}