#ifndef PERF_TEST_H__
#define PERF_TEST_H__

#include "sparse_vec.h"
#include "sparse_matr.h"
#include "errors.h"
#include "stddef.h"

int bin_search_ind(size_t target, vec_elem_t *elems, size_t size);

error mult_mat_vec(sparse_matrix *sm, sparse_vector *sv, sparse_vector *res);

#endif