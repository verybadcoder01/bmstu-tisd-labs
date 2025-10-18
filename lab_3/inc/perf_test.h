#ifndef PERF_TEST_H__
#define PERF_TEST_H__

#include "sparse_vec.h"
#include "sparse_matr.h"
#include "errors.h"
#include "stddef.h"
#include "regular_matr.h"
#include "time.h"

int bin_search_ind(size_t target, vec_elem_t *elems, size_t size);

error mult_sp_mat_sp_vec_clean(sparse_matrix *sm, sparse_vector *sv, sparse_vector *res);

error mult_sp_mat_sp_vec(sparse_matrix *sm, sparse_vector *sv, sparse_vector *res);

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end);

error measure_sp_mult_time(sparse_matrix *sm, sparse_vector *sv, double *res_clean, double *res_alloc);

error measure_reg_mult_time(matrix *m, vector *v, double *res);

error run_perf_test();

#endif