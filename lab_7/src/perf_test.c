#include "perf_test.h"
#include "errors.h"
#include "hash_table.h"
#include "node.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "tree.h"
#include <bits/time.h>

const int count = 7;
const int sizes[] = { 100, 200, 500, 1000, 2000, 5000, 10000 };
const int perf_runs = 15;

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) * (unsigned long)1e9 + (end->tv_nsec - start->tv_nsec);
}

error measure_search_ht_open(hash_fn hash)
{
    for (int i = 0; i < count; ++i)
    {
        hash_table_open ht_open;
        srand(time(NULL));
        error rc = ht_open_init(&ht_open, sizes[i], MAX_OPEN_LOAD_FACTOR, hash, second_hash_func, second_hash_probe);
        if (rc)
        {
            ht_open_delete(&ht_open);
            return rc;
        }
        int *keys = malloc(sizeof(int) * sizes[i]);
        if (!keys)
        {
            ht_open_delete(&ht_open);
            return ALLOC_ERROR;
        }
        for (int j = 0; j < sizes[i]; ++j)
        {
            int key = rand();
            rc = ht_open_insert(&ht_open, key);
            while (rc == DUPLICATE_KEY)
            {
                key = rand();
                rc = ht_open_insert(&ht_open, key);
            }
            if (rc && rc != DUPLICATE_KEY)
            {
                ht_open_delete(&ht_open);
                free(keys);
                return rc;
            }
            keys[j] = key;
        }
        double total_search = 0;
        double total_cmps = 0;
        for (int run = 0; run < perf_runs; ++run)
        {
            unsigned int cur_run = 0;
            unsigned int cur_cmps = 0;
            for (int j = 0; j < sizes[i]; ++j)
            {
                int cmp_count = 0;
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                rc = ht_open_find(&ht_open, keys[j], &cmp_count);
                clock_gettime(CLOCK_MONOTONIC, &end);
                cur_run += elapsed_time(&start, &end);
                cur_cmps += cmp_count;
            }
            total_search += (double)cur_run / (double)sizes[i];
            total_cmps += (double)cur_cmps / (double)sizes[i];
        }
        printf("Поиск в хэш-таблице с открытой адресацией на %d элементах: %lf\n", sizes[i], total_search / (double)perf_runs);
        printf("В среднем потребовалось %lf сравнений\n", total_cmps / (double)perf_runs);
        ht_open_delete(&ht_open);
        free(keys);
    }
    return 0;
}

error measure_search_ht_chain(hash_fn hash)
{
    for (int i = 0; i < count; ++i)
    {
        hash_table_chain ht_chain;
        srand(time(NULL));
        error rc = ht_chain_init(&ht_chain, sizes[i], 1.2, hash);
        if (rc)
        {
            ht_chain_delete(&ht_chain);
            return rc;
        }
        int *keys = malloc(sizeof(int) * sizes[i]);
        if (!keys)
        {
            ht_chain_delete(&ht_chain);
            return ALLOC_ERROR;
        }
        for (int j = 0; j < sizes[i]; ++j)
        {
            int key = rand();
            rc = ht_chain_insert(&ht_chain, key);
            while (rc == DUPLICATE_KEY)
            {
                key = rand();
                rc = ht_chain_insert(&ht_chain, key);
            }
            if (rc && rc != DUPLICATE_KEY)
            {
                ht_chain_delete(&ht_chain);
                free(keys);
                return rc;
            }
            keys[j] = key;
        }
        double total_search = 0;
        double total_cmps = 0;
        for (int run = 0; run < perf_runs; ++run)
        {
            unsigned int cur_run = 0;
            unsigned int cur_cmps = 0;
            for (int j = 0; j < sizes[i]; ++j)
            {
                int cmp_count = 0;
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                rc = ht_chain_find(&ht_chain, keys[j], &cmp_count);
                clock_gettime(CLOCK_MONOTONIC, &end);
                cur_cmps += cmp_count;
                cur_run += elapsed_time(&start, &end);
            }
            total_search += (double)cur_run / (double)sizes[i];
            total_cmps += (double)cur_cmps / (double)sizes[i];
        }
        printf("Поиск в хэш-таблице с методом цепочек на %d элементах: %lf\n", sizes[i], total_search / (double)perf_runs);
        printf("В среднем потребовалось %lf сравнений\n", total_cmps / (double)perf_runs);
        ht_chain_delete(&ht_chain);
        free(keys);
    }
    return 0;
}

error measure_search_avl_tree()
{
    for (int i = 0; i < count; ++i)
    {
        node_t *root = NULL;
        srand(time(NULL));
        int *keys = malloc(sizeof(int) * sizes[i]);
        if (!keys)
        {
            tree_delete(root);
            return ALLOC_ERROR;
        }
        error rc = 0;
        for (int j = 0; j < sizes[i]; ++j)
        {
            int key = rand();
            rc = tree_insert(&root, comp_node_data, key);
            while (rc == DUPLICATE_KEY)
            {
                key = rand();
                rc = tree_insert(&root, comp_node_data, key);
            }
            if (rc && rc != DUPLICATE_KEY)
            {
                tree_delete(root);
                free(keys);
                return rc;
            }
            keys[j] = key;
        }
        double total_search = 0;
        double total_cmps = 0;
        for (int run = 0; run < perf_runs; ++run)
        {
            unsigned int cur_run = 0;
            unsigned int cur_cmps = 0;
            for (int j = 0; j < sizes[i]; ++j)
            {
                int cmp_count = 0;
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                rc = tree_find(root, comp_node_data, &keys[j], &cmp_count);
                clock_gettime(CLOCK_MONOTONIC, &end);
                cur_run += elapsed_time(&start, &end);
                cur_cmps += cmp_count;
            }
            total_search += (double)cur_run / (double)sizes[i];
            total_cmps += (double)cur_cmps / (double)sizes[i];
        }
        printf("Поиск в AVL-дереве на %d элементах: %lf\n", sizes[i], total_search / (double)perf_runs);
        printf("В среднем потребовалось %lf сравнений\n", total_cmps / (double)perf_runs);
        tree_delete(root);
        free(keys);
    }
    return 0;
}

error measure_insert_ht_open(hash_fn hash)
{
    for (int i = 0; i < count; ++i)
    {
        hash_table_open ht_open;
        srand(time(NULL));
        double total_insert = 0;
        int max_size = 0;
        for (int run = 0; run < perf_runs; ++run)
        {
            unsigned int cur_run = 0;
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            error rc = ht_open_init(&ht_open, sizes[i], MAX_OPEN_LOAD_FACTOR, hash, second_hash_func, second_hash_probe);
            clock_gettime(CLOCK_MONOTONIC, &end);
            if (rc)
            {
                ht_open_delete(&ht_open);
                return rc;
            }
            cur_run += elapsed_time(&start, &end);
            for (int j = 0; j < sizes[i]; ++j)
            {
                int key = rand();
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                rc = ht_open_insert(&ht_open, key);
                clock_gettime(CLOCK_MONOTONIC, &end);
                if (rc && rc != DUPLICATE_KEY)
                {
                    ht_open_delete(&ht_open);
                    return rc;
                }
                cur_run += elapsed_time(&start, &end);
            }
            total_insert += (double)cur_run / (double)sizes[i];
            max_size = ht_open.size > max_size ? ht_open.size : max_size;
            ht_open_delete(&ht_open);
        }
        printf("Добавление в хэш-таблице с открытой адресацией на %d элементах: %lf\n", sizes[i], total_insert / (double)perf_runs);
        printf("Размер: %zu байт\n", max_size * sizeof(htable_elem_t));
    }
    return 0;
}

error measure_insert_ht_chain(hash_fn hash)
{
    for (int i = 0; i < count; ++i)
    {
        hash_table_chain ht_chain;
        srand(time(NULL));
        double total_insert = 0;
        int max_size = 0;
        for (int run = 0; run < perf_runs; ++run)
        {
            unsigned int cur_run = 0;
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            error rc = ht_chain_init(&ht_chain, sizes[i], 1.2, hash);
            clock_gettime(CLOCK_MONOTONIC, &end);
            if (rc)
            {
                ht_chain_delete(&ht_chain);
                return rc;
            }
            cur_run += elapsed_time(&start, &end);
            for (int j = 0; j < sizes[i]; ++j)
            {
                int key = rand();
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                rc = ht_chain_insert(&ht_chain, key);
                clock_gettime(CLOCK_MONOTONIC, &end);
                if (rc && rc != DUPLICATE_KEY)
                {
                    ht_chain_delete(&ht_chain);
                    return rc;
                }
                cur_run += elapsed_time(&start, &end);
            }
            total_insert += (double)cur_run / (double)sizes[i];
            max_size = ht_chain.count > max_size ? ht_chain.count : max_size;
            ht_chain_delete(&ht_chain);
        }
        printf("Добавление в хэш-таблице с методом цепочек на %d элементах: %lf\n", sizes[i], total_insert / (double)perf_runs);
        printf("Размер: %zu байт\n", max_size * sizeof(htable_node_t));
    }
    return 0;
}

error measure_insert_avl_tree()
{
    for (int i = 0; i < count; ++i)
    {
        srand(time(NULL));
        double total_insert = 0;
        for (int run = 0; run < perf_runs; ++run)
        {
            node_t *root = NULL;
            unsigned int cur_run = 0;
            error rc = 0;
            for (int j = 0; j < sizes[i]; ++j)
            {
                int key = rand();
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                rc = tree_insert(&root, comp_node_data, key);
                clock_gettime(CLOCK_MONOTONIC, &end);
                if (rc && rc != DUPLICATE_KEY)
                {
                    tree_delete(root);
                    return rc;
                }
                cur_run += elapsed_time(&start, &end);
            }
            total_insert += (double)cur_run / (double)sizes[i];
            tree_delete(root);
        }
        printf("Добавление в avl-дереве на %d элементах: %lf\n", sizes[i], total_insert / (double)perf_runs);
        printf("Размер: %zu байт\n", sizes[i] * sizeof(node_t));
    }
    return 0;
}

error run_perf_test()
{
    printf("С функцией хеширования Кнута:\n");   
    error rc = measure_search_ht_chain(hash_knuth_xor);
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n");
    rc = measure_insert_ht_chain(hash_knuth_xor);
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n");
    rc = measure_search_ht_open(hash_knuth_xor);
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n");
    rc = measure_insert_ht_open(hash_knuth_xor);
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n\n");

    printf("С функцией хеширования через золотое сечение:\n");   
    rc = measure_search_ht_chain(hash_mult);
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n");
    rc = measure_insert_ht_chain(hash_mult);
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n");
    rc = measure_search_ht_open(hash_mult);
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n");
    rc = measure_insert_ht_open(hash_mult);
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n\n");
    
    rc = measure_search_avl_tree();
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n");
    rc = measure_insert_avl_tree();
    if (rc)
    {
        return rc;
    }
    printf("-----------------------------------------------------------\n");
    return 0;
}