#include "perf_test.h"
#include "math.h"
#include "stdlib.h"

#define PERF_TEST_CNT 7
const char PERF_TEST_FILES_SP[PERF_TEST_CNT][40] = {"perf/1_perc.txt", "perf/10_perc.txt", "perf/30_perc.txt", "perf/50_perc.txt", "perf/70_perc.txt", "perf/80_perc.txt", "perf/100_perc.txt"};
const char PERF_TEST_FILES_REG[PERF_TEST_CNT][40] = {"perf/1_perc_reg.txt", "perf/10_perc_reg.txt", "perf/30_perc_reg.txt", "perf/50_perc_reg.txt", "perf/70_perc_reg.txt", "perf/80_perc_reg.txt", "perf/100_perc_reg.txt"};
const int PERF_TEST_RUNS = 15;

error mult_sp_mat_sp_vec(sparse_matrix *sm, sparse_vector *sv, sparse_vector *res)
{
    if (sm->true_cols != sv->true_len)
    {
        return INCOMPATIBLE_SIZES;
    }
    double *vec_dense = calloc(sm->true_cols, sizeof(double));
    for (size_t i = 0; i < sv->elem_cnt; ++i)
    {
        vec_dense[sv->vec[i].ind] = sv->vec[i].data;
    }
    double *res_dense = calloc(sm->true_rows, sizeof(double));

    for (size_t i = 0; i < sm->rows_cnt; ++i)
    {
        size_t row_beg = sm->row_start_inds[i];
        size_t row_end = (i < sm->rows_cnt - 1) ? sm->row_start_inds[i + 1] : sm->mat_elems_cnt;

        for (size_t j = row_beg; j < row_end; ++j)
        {
            res_dense[i] += sm->mat_straight[j].data * vec_dense[sm->mat_straight[j].col_ind];
        }
    }

    res->true_len = sm->true_rows;
    res->vec = malloc(sm->true_rows * sizeof(vec_elem_t));
    if (!res->vec)
    {
        free(res_dense);
        free(vec_dense);
        return ALLOC_ERROR;
    }

    res->elem_cap = sm->true_rows;
    res->elem_cnt = 0;
    for (size_t i = 0; i < sm->true_rows; ++i)
    {
        if (fabs(res_dense[i]) > 1e-6)
        {
            res->vec[res->elem_cnt].data = res_dense[i];
            res->vec[res->elem_cnt].ind = i;
            res->elem_cnt++;
        }
    }
    free(res_dense);
    free(vec_dense);
    return 0;
}

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end)
{
    return (end->tv_sec * 1000000 + end->tv_nsec / 1000) -
           (start->tv_sec * 1000000 + start->tv_nsec / 1000);
}

error measure_reg_mult_time(matrix *m, vector *v, double *res)
{
    *res = 0;
    for (int _ = 0; _ < PERF_TEST_RUNS; ++_)
    {
        vector *tmp = NULL;
        error rc = empty_vector(&tmp);
        if (rc)
        {
            delete_vector(tmp);
            return rc;
        }
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        rc = mult_regular_matr_vec(m, v, tmp);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        delete_vector(tmp);
        if (rc)
        {
            return rc;
        }
        *res += elapsed_time(&start, &end);
    }
    *res /= (double)PERF_TEST_RUNS;
    return 0;
}

error measure_sp_mult_time(sparse_matrix *sm, sparse_vector *sv, double *res)
{
    *res = 0;
    for (int _ = 0; _ < PERF_TEST_RUNS; ++_)
    {
        sparse_vector *tmp = NULL;
        error rc = empty_sp_vector(&tmp);
        if (rc)
        {
            delete_sp_vec(tmp);
            return rc;
        }
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        rc = mult_sp_mat_sp_vec(sm, sv, tmp);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        delete_sp_vec(tmp);
        if (rc)
        {
            return rc;
        }
        *res += elapsed_time(&start, &end);
    }
    *res /= (double)PERF_TEST_RUNS;
    return 0;
}

error run_perf_test()
{
    for (int i = 0; i < PERF_TEST_CNT; ++i)
    {
        FILE *f = fopen(PERF_TEST_FILES_SP[i], "r");
        if (!f)
        {
            return PERF_TEST_ERROR;
        }
        sparse_matrix *sm = NULL;
        error rc = empty_sp_matrix(&sm);
        if (rc)
        {
            fclose(f);
            delete_sp_matrix(sm);
            return rc;
        }
        rc = init_sp_matrix_from_file(sm, f);
        sparse_vector *sv = NULL;
        rc = empty_sp_vector(&sv);
        if (rc)
        {
            fclose(f);
            delete_sp_matrix(sm);
            delete_sp_vec(sv);
            return rc;
        }
        rc = init_sp_vector_from_file(sv, f);
        if (rc)
        {
            fclose(f);
            delete_sp_matrix(sm);
            delete_sp_vec(sv);
            return rc;
        }
        double res_sparse = 0;
        rc = measure_sp_mult_time(sm, sv, &res_sparse);
        fclose(f);
        delete_sp_matrix(sm);
        delete_sp_vec(sv);
        if (rc)
        {
            return rc;
        }
        printf("Время умножения разреженным способом на файле %s: %lf\n", PERF_TEST_FILES_SP[i], res_sparse);
        FILE *fr = fopen(PERF_TEST_FILES_REG[i], "r");
        if (!fr)
        {
            return PERF_TEST_ERROR;
        }
        matrix *m = NULL;
        rc = empty_matrix(&m);
        if (rc)
        {
            fclose(fr);
            delete_matrix(m);
            return rc;
        }
        rc = init_matrix_from_file(m, fr);
        if (rc)
        {
            fclose(fr);
            delete_matrix(m);
            return rc;
        }
        vector *v = NULL;
        rc = empty_vector(&v);
        if (rc)
        {
            fclose(fr);
            delete_matrix(m);
            delete_vector(v);
            return rc;
        }
        rc = init_vector_from_file(v, fr);
        if (rc)
        {
            fclose(fr);
            delete_matrix(m);
            delete_vector(v);
            return rc;
        }
        double res_reg = 0;
        rc = measure_reg_mult_time(m, v, &res_reg);
        fclose(fr);
        delete_matrix(m);
        delete_vector(v);
        if (rc)
        {
            return rc;
        }
        printf("Время умножения обычным способом на файле %s: %lf\n", PERF_TEST_FILES_REG[i], res_reg);
    }
    return 0;
}
