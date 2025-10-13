#ifndef PERF_TEST_H__
#define PERF_TEST_H__

#include "sparse_vec.h"
#include "sparse_matr.h"
#include "errors.h"
#include "stddef.h"
#include "regular_matr.h"
#include "time.h"

error mult_sp_mat_sp_vec(sparse_matrix *sm, sparse_vector *sv, sparse_vector *res);

unsigned long elapsed_time(const struct timespec *start, const struct timespec *end);

error measure_sp_mult_time(sparse_matrix *sm, sparse_vector *sv, double *res);

error measure_reg_mult_time(matrix *m, vector *v, double *res);

error run_perf_test();

#endif