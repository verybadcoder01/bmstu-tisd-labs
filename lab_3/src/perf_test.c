#include "perf_test.h"
#include "math.h"
#include "stdlib.h"

// clang-format off
#define PERF_TEST_CNT 28
const char PERF_TEST_FILES_SP[PERF_TEST_CNT][40] = {
    "perf/1_perc_500.txt", "perf/1_perc_1000.txt", "perf/1_perc_2000.txt", "perf/1_perc_5000.txt",
    "perf/10_perc_500.txt", "perf/10_perc_1000.txt", "perf/10_perc_2000.txt", "perf/10_perc_5000.txt",
    "perf/30_perc_500.txt", "perf/30_perc_1000.txt", "perf/30_perc_2000.txt", "perf/30_perc_5000.txt",
    "perf/50_perc_500.txt", "perf/50_perc_1000.txt", "perf/50_perc_2000.txt", "perf/50_perc_5000.txt",
    "perf/70_perc_500.txt", "perf/70_perc_1000.txt", "perf/70_perc_2000.txt", "perf/70_perc_5000.txt",
    "perf/80_perc_500.txt", "perf/80_perc_1000.txt", "perf/80_perc_2000.txt", "perf/80_perc_5000.txt",
    "perf/100_perc_500.txt", "perf/100_perc_1000.txt", "perf/100_perc_2000.txt", "perf/100_perc_5000.txt",
};
const char PERF_TEST_FILES_REG[PERF_TEST_CNT][40] = {
    "perf/1_perc_500_reg.txt", "perf/1_perc_1000_reg.txt", "perf/1_perc_2000_reg.txt", "perf/1_perc_5000_reg.txt",
    "perf/10_perc_500_reg.txt", "perf/10_perc_1000_reg.txt", "perf/10_perc_2000_reg.txt", "perf/10_perc_5000_reg.txt",
    "perf/30_perc_500_reg.txt", "perf/30_perc_1000_reg.txt", "perf/30_perc_2000_reg.txt", "perf/30_perc_5000_reg.txt",
    "perf/50_perc_500_reg.txt", "perf/50_perc_1000_reg.txt", "perf/50_perc_2000_reg.txt", "perf/50_perc_5000_reg.txt",
    "perf/70_perc_500_reg.txt", "perf/70_perc_1000_reg.txt", "perf/70_perc_2000_reg.txt", "perf/70_perc_5000_reg.txt",
    "perf/80_perc_500_reg.txt", "perf/80_perc_1000_reg.txt", "perf/80_perc_2000_reg.txt", "perf/80_perc_5000_reg.txt",
    "perf/100_perc_500_reg.txt", "perf/100_perc_1000_reg.txt", "perf/100_perc_2000_reg.txt", "perf/100_perc_5000_reg.txt"};
const int PERF_TEST_RUNS = 15;
// clang-format on

int bin_search_ind(size_t target, vec_elem_t *elems, size_t size)
{
    int l = 0, r = (int)size;
    while (r - l > 1)
    {
        int mid = (l + r) / 2;
        if (elems[mid].ind > target)
        {
            r = mid;
        }
        else
        {
            l = mid;
        }
    }
    if (elems[l].ind == target)
    {
        return l;
    }
    else if (r < (int)size && elems[r].ind == target)
    {
        return r;
    }
    return -1;
}

error mult_sp_mat_sp_vec_clean(sparse_matrix *sm, sparse_vector *sv, sparse_vector *res)
{
    if (sm->true_cols != sv->true_len)
    {
        return INCOMPATIBLE_SIZES;
    }
    res->true_len = sv->true_len;
    for (size_t i = 0; i < sm->rows_cnt; ++i)
    {
        size_t row_desc_beg = sm->row_start_inds[i];
        size_t row_desc_end = (i < sm->rows_cnt - 1) ? sm->row_start_inds[i + 1] : sm->mat_elems_cnt;
        double cur_data = 0;
        for (size_t j = row_desc_beg; j < row_desc_end; ++j)
        {
            int ind = bin_search_ind(sm->mat_straight[j].col_ind, sv->vec, sv->elem_cnt);
            if (ind != -1)
            {
                cur_data += sm->mat_straight[j].data * sv->vec[ind].data;
            }
        }
        if (fabs(cur_data) > 1e-6)
        {
            vec_elem_t n_elem = {cur_data, i};
            error rc = add_vec_elem(res, &n_elem);
            if (rc)
            {
                return rc;
            }
        }
    }
    return 0;
}

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
    res->true_len = sm->true_rows;
    res->elem_cnt = 0;
    res->elem_cap = 0;
    for (size_t i = 0; i < sm->rows_cnt; ++i)
    {
        size_t row_beg = sm->row_start_inds[i];
        size_t row_end = (i < sm->rows_cnt - 1) ? sm->row_start_inds[i + 1] : sm->mat_elems_cnt;
        double result = 0;
        for (size_t j = row_beg; j < row_end; ++j)
        {
            result += sm->mat_straight[j].data * vec_dense[sm->mat_straight[j].col_ind];
        }
        if (fabs(result) > 1e-6)
        {
            vec_elem_t cur_res = {result, i};
            error rc = add_vec_elem(res, &cur_res);
            if (rc)
            {
                free(vec_dense);
                return rc;
            }
        }
    }
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

error measure_sp_mult_time(sparse_matrix *sm, sparse_vector *sv, double *res_clean, double *res_alloc)
{
    *res_clean = 0;
    *res_alloc = 0;
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
        if (rc)
        {
            delete_sp_vec(tmp);
            return rc;
        }
        rc = empty_sp_vector(&tmp);
        if (rc)
        {
            delete_sp_vec(tmp);
            return rc;
        }
        *res_alloc += elapsed_time(&start, &end);
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        rc = mult_sp_mat_sp_vec_clean(sm, sv, tmp);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        delete_sp_vec(tmp);
        if (rc)
        {
            return rc;
        }
        *res_clean += elapsed_time(&start, &end);
    }
    *res_clean /= (double)PERF_TEST_RUNS;
    *res_alloc /= (double)PERF_TEST_RUNS;
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
        rc = init_sp_vector_from_file(sv, f, sm->true_cols);
        if (rc)
        {
            fclose(f);
            delete_sp_matrix(sm);
            delete_sp_vec(sv);
            return rc;
        }
        double res_sparse_clean = 0, res_sparse_alloc = 0;
        rc = measure_sp_mult_time(sm, sv, &res_sparse_clean, &res_sparse_alloc);
        fclose(f);
        delete_sp_matrix(sm);
        delete_sp_vec(sv);
        if (rc)
        {
            return rc;
        }
        printf("Время умножения разреженным способом без выделения на файле %s: %lf\n", PERF_TEST_FILES_SP[i], res_sparse_clean);
        printf("Время умножения разреженным способом с выделением на файле %s: %lf\n", PERF_TEST_FILES_SP[i], res_sparse_alloc);
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
