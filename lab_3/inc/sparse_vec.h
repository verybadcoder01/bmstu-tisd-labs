#ifndef SPARSE_VEC_H__
#define SPARSE_VEC_H__

#include "stddef.h"
#include "stdio.h"
#include "errors.h"

typedef struct {
    double data;
    size_t ind;
} vec_elem_t;

typedef struct
{
    vec_elem_t *vec;
    size_t elem_cnt;
    size_t elem_cap;
    size_t true_len;
} sparse_vector;

int comp_vec_elems(const void *l, const void *r);

error add_vec_elem(sparse_vector *sv, vec_elem_t *val);

error build_sp_vec(sparse_vector *sv, vec_elem_t *val, size_t count);

error init_sp_vector_from_file(sparse_vector *sv, FILE *f, size_t m_cols);

error empty_sp_vector(sparse_vector **sv);

void delete_sp_vec(sparse_vector *sv);

void print_sp_vec(sparse_vector *sv);

#endif