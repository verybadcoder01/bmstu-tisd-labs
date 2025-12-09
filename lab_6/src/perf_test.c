#include <time.h>
#define _GNU_SOURCE
#include "errors.h"
#include "node.h"
#include "perf_test.h"
#include "string.h"
#include "time.h"
#include "tree.h"
#include <bits/time.h>
#include <stdio.h>
#include <stdlib.h>

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end)
{
    return (end->tv_sec - start->tv_sec) * (unsigned long)1e9 + (end->tv_nsec - start->tv_nsec);
}

void generate_random_string(char *str, int length, int ind)
{
    snprintf(str, length + 1, "%0*d", length, ind);
}

static void memswap(void *a, void *b, size_t size)
{
    char *ca = (char *)a;
    char *cb = (char *)b;

    for (size_t i = 0; i < size; i++)
    {
        char temp = ca[i];
        ca[i] = cb[i];
        cb[i] = temp;
    }
}

void shuffle(void *array, size_t n_elems, size_t elem_size)
{
    if (n_elems > 1)
    {
        char *base = (char *)array;
        for (size_t i = n_elems - 1; i > 0; i--)
        {
            size_t j = (size_t)((double)rand() / ((double)RAND_MAX + 1) * (i + 1));
            memswap(base + i * elem_size, base + j * elem_size, elem_size);
        }
    }
}

void generate_balanced_recursive(char **sorted, char **result,
                                 int start, int end, int *index)
{
    if (start > end)
    {
        return;
    }
    int mid = start + (end - start) / 2;
    result[(*index)++] = strdup(sorted[mid]);
    generate_balanced_recursive(sorted, result, start, mid - 1, index);
    generate_balanced_recursive(sorted, result, mid + 1, end, index);
}

void generate_balanced_gpa(double *sorted, double *result, int start, int end, int *index)
{
    if (start > end)
    {
        return;
    }
    int mid = start + (end - start) / 2;
    result[(*index)++] = sorted[mid];
    generate_balanced_gpa(sorted, result, start, mid - 1, index);
    generate_balanced_gpa(sorted, result, mid + 1, end, index);
}

int comp_strs(const void *l, const void *r)
{
    return strcmp((const char *)l, (const char *)r);
}

error gen_left_tree(node_t **root, char **keys, size_t size, comp_fn comp)
{
    for (size_t i = 0; i < size; ++i)
    {
        error rc = tree_insert(root, comp, keys[i], (double)i);
        if (rc)
        {
            return rc;
        }
    }
    return 0;
}

error gen_ideal_tree(node_t **root, char **keys, size_t size)
{
    char **result = malloc(sizeof(char *) * size);
    if (!result)
    {
        return ALLOC_ERROR;
    }
    int index = 0;
    generate_balanced_recursive(keys, result, 0, size - 1, &index);
    for (size_t i = 0; i < size; ++i)
    {
        error rc = tree_insert(root, comp_node_surname, result[i], (double)i);
        if (rc)
        {
            for (size_t i = 0; i < size; ++i)
            {
                free(result[i]);
            }
            free(result);
            return rc;
        }
    }
    for (size_t i = 0; i < size; ++i)
    {
        free(result[i]);
    }
    free(result);
    return 0;
}

error gen_ideal_tree_by_gpa(node_t **root, double *keys, double *result, size_t size)
{
    int index = 0;
    generate_balanced_gpa(keys, result, 0, size - 1, &index);
    for (size_t i = 0; i < size; ++i)
    {
        error rc = tree_insert(root, comp_node_gpa, "some_string", result[i]);
        if (rc)
        {
            free(keys);
            free(result);
            return rc;
        }
    }
    return 0;
}

void graphviz_check(node_t *root_ideal, node_t *root_left, size_t size)
{
    char *filenames[2] = { NULL, NULL };
    asprintf(&filenames[0], "ideal%zu.dot", size);
    asprintf(&filenames[1], "left%zu.dot", size);
    FILE *fout_ideal = fopen(filenames[0], "w"), *fout_left = fopen(filenames[1], "w");
    serialize_to_graphviz(root_ideal, fout_ideal);
    serialize_to_graphviz(root_left, fout_left);
    fclose(fout_ideal);
    fclose(fout_left);
    free(filenames[0]);
    free(filenames[1]);
}

error measure_worst_sort(int size)
{
    node_t *root = NULL;
    struct timespec start, end;
    char **dummy = malloc(sizeof(char *) * size);
    double *dummy_gpa = malloc(sizeof(double) * size);
    if (!dummy || !dummy_gpa)
    {
        tree_delete(root);
        return ALLOC_ERROR;
    }
    unsigned long total_time = 0;
    for (int run = 0; run < 15; ++run)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = size - 1; i >= 0; --i)
        {
            error rc = tree_insert(&root, comp_node_gpa, "a", (double)i);
            if (rc)
            {
                tree_delete(root);
                free(dummy);
                free(dummy_gpa);
                return rc;
            }
        }
        tree_to_sorted_array(root, dummy, dummy_gpa, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_time += elapsed_time(&start, &end);
        tree_delete(root);
        root = NULL;
    }
    printf("Среднее время сортировки %d уже отсортированных элементов: %lf\n", size, ((double)total_time / (double)15) * 0.000001);
    free(dummy);
    free(dummy_gpa);
    return 0;
}

error measure_sort(size_t size)
{
    struct timespec start, end;
    node_t *root = NULL;
    double *keys = malloc(sizeof(double) * size);
    if (!keys)
    {
        return ALLOC_ERROR;
    }
    for (size_t i = 0; i < size; ++i)
    {
        keys[i] = (double)i;
    }
    unsigned long total_time = 0;
    shuffle(keys, size, sizeof(double));
    for (int run = 0; run < 10; ++run)
    {
        unsigned long cur_time = 0;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (size_t i = 0; i < size; ++i)
        {
            error rc = tree_insert(&root, comp_node_gpa, "a", keys[i]);
            if (rc)
            {
                tree_delete(root);
                free(keys);
                return rc;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        cur_time += elapsed_time(&start, &end);

        char **dummy = malloc(sizeof(char *) * size);
        double *dummy_gpa = malloc(sizeof(double) * size);
        if (!dummy || !dummy_gpa)
        {
            tree_delete(root);
            free(keys);
            return ALLOC_ERROR;
        }

        clock_gettime(CLOCK_MONOTONIC, &start);
        tree_to_sorted_array(root, dummy, dummy_gpa, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);
        cur_time += elapsed_time(&start, &end);
        total_time += cur_time;
        free(dummy);
        free(dummy_gpa);
        tree_delete(root);
        root = NULL;
    }
    printf("Среднее время сортирвоки %zu элементов: %lf\n", size, ((double)total_time / (double)10) * 0.000001);
    free(keys);
    return 0;
}

error run_perf_test()
{
    node_t *root_left = NULL, *root_ideal = NULL;
    const int sizes[] = { 100, 500, 1000, 2000, 5000, 10000 };
    srand(time(NULL));
    for (int i = 0; i < 6; ++i)
    {
        char **keys = malloc(sizeof(char *) * sizes[i]);
        if (!keys)
        {
            return ALLOC_ERROR;
        }
        const int key_len = 10;
        for (int j = 0; j < sizes[i]; ++j)
        {
            keys[j] = malloc(key_len + 1);
            if (!keys[j])
            {
                return ALLOC_ERROR;
            }
            generate_random_string(keys[j], key_len, j);
        }
        error rc = gen_left_tree(&root_left, keys, sizes[i], comp_node_surname);
        if (rc)
        {
            tree_delete(root_left);
            return PERF_TEST_ERROR;
        }
        rc = gen_ideal_tree(&root_ideal, keys, sizes[i]);
        if (rc)
        {
            tree_delete(root_left);
            tree_delete(root_ideal);
            return PERF_TEST_ERROR;
        }

        unsigned long total_search_left = 0, total_search_ideal = 0;
        unsigned long total_removal_by_surname_left = 0;
        unsigned long total_removal_by_surname_ideal = 0;
        // замер поиска
        for (int key_ind = 0; key_ind < sizes[i]; ++key_ind)
        {
            node_t *cur = NULL;
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            rc = tree_find(root_left, comp_node_surname, keys[key_ind], &cur);
            clock_gettime(CLOCK_MONOTONIC, &end);
            total_search_left += elapsed_time(&start, &end);
            clock_gettime(CLOCK_MONOTONIC, &start);
            rc = tree_find(root_ideal, comp_node_surname, keys[key_ind], &cur);
            clock_gettime(CLOCK_MONOTONIC, &end);
            total_search_ideal += elapsed_time(&start, &end);
        }
        double avg_search_left = (double)total_search_left / (double)sizes[i], avg_search_ideal = (double)total_search_ideal / (double)sizes[i];
        printf("Среднее время поиска на %d вершинах в вырожденном дереве %lf, в идеальном дереве %lf\n", sizes[i], avg_search_left, avg_search_ideal);
        graphviz_check(root_ideal, root_left, sizes[i]);

        // замер удаления
        shuffle(keys, sizes[i], sizeof(char *));
        for (int key_ind = 0; key_ind < sizes[i]; ++key_ind)
        {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            rc = tree_remove(&root_left, comp_node_surname, keys[i]);
            clock_gettime(CLOCK_MONOTONIC, &end);
            total_removal_by_surname_left += elapsed_time(&start, &end);
            clock_gettime(CLOCK_MONOTONIC, &start);
            rc = tree_remove(&root_ideal, comp_node_surname, keys[i]);
            clock_gettime(CLOCK_MONOTONIC, &end);
            total_removal_by_surname_ideal += elapsed_time(&start, &end);
        }
        double avg_removal_left = (double)total_removal_by_surname_left / (double)sizes[i], avg_removal_ideal = (double)total_removal_by_surname_ideal / (double)sizes[i];
        printf("Среднее время удаления по фамилии на %d вершинах в вырожденном дереве %lf, в идеальном %lf\n", sizes[i], avg_removal_left, avg_removal_ideal);

        // замер построения и удаления
        unsigned long total_rebuild_time_left = 0, total_rebuild_time_ideal = 0;
        unsigned long total_remove_left = 0, total_remove_ideal = 0;
        double *keys_gpa = malloc(sizeof(double) * sizes[i]), *result_gpa = malloc(sizeof(double) * sizes[i]);
        double *keys_gpa_shuffled = malloc(sizeof(double) * sizes[i]);
        if (!keys_gpa || !result_gpa || !keys_gpa_shuffled)
        {
            return ALLOC_ERROR;
        }
        for (int j = 0; j < sizes[i]; ++j)
        {
            keys_gpa[j] = (double)j;
            keys_gpa_shuffled[j] = (double)j;
        }
        shuffle(keys_gpa_shuffled, sizes[i], sizeof(double));
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        // левое дерево
        rc = gen_left_tree(&root_left, keys, sizes[i], comp_node_gpa);
        if (rc)
        {
            tree_delete(root_left);
            tree_delete(root_ideal);
            return PERF_TEST_ERROR;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_rebuild_time_left += elapsed_time(&start, &end);
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int j = 0; j < sizes[i]; ++j)
        {
            double val = (double)keys_gpa_shuffled[j];
            rc = tree_remove(&root_left, comp_node_gpa, &val);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_remove_left += elapsed_time(&start, &end);
        // идеальное
        for (int j = 0; j < sizes[j]; ++j)
        {
            keys_gpa[j] = (double)j;
        }
        clock_gettime(CLOCK_MONOTONIC, &start);
        rc = gen_ideal_tree_by_gpa(&root_ideal, keys_gpa, result_gpa, sizes[i]);
        if (rc)
        {
            tree_delete(root_left);
            tree_delete(root_ideal);
            return PERF_TEST_ERROR;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_rebuild_time_ideal += elapsed_time(&start, &end);
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int j = 0; j < sizes[i]; ++j)
        {
            double val = (double)keys_gpa_shuffled[j];
            rc = tree_remove(&root_left, comp_node_gpa, &val);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_remove_ideal += elapsed_time(&start, &end);
        double avg_remove_left = (double)total_remove_left / (double)sizes[i], avg_remove_ideal = (double)total_remove_ideal / (double)sizes[i];
        printf("Время перестроения по баллу на %d вершинах для вырожденного дерева %zu, для идеального %zu\n", sizes[i], total_rebuild_time_left, total_rebuild_time_ideal);
        printf("Среднее время удаления по баллу на %d вершинах для вырожденного дерева %lf, для идеального %lf\n", sizes[i], avg_remove_left, avg_remove_ideal);
        // очистка
        tree_delete(root_ideal);
        tree_delete(root_left);
        root_ideal = NULL;
        root_left = NULL;
        for (int j = 0; j < sizes[i]; ++j)
        {
            free(keys[j]);
        }
        free(keys);
        free(keys_gpa);
        free(result_gpa);
        free(keys_gpa_shuffled);
        printf("---------------------------------------------------------------------------------------------\n");
    }
    printf("Замеры сортировки:\n");
    for (int i = 0; i < 6; ++i)
    {
        measure_sort(sizes[i]);
        measure_worst_sort(sizes[i]);
    }
    return 0;
}
