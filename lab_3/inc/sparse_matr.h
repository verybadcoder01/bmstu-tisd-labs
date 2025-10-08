#ifndef SPARSE_MATR_H__
#define SPARSE_MATR_H__

#include "stdio.h"
#include "errors.h"

typedef struct
{
    double data;
    size_t col_ind;
} matrix_elem_t;

typedef struct
{
    matrix_elem_t *mat_straight;
    size_t mat_elems_cnt, mat_elems_cap;
    size_t *row_start_inds;
    size_t rows_cnt, rows_cap;
} sparse_matrix;

int comp_matr_elems(const void *l, const void *r);

error push_elem_to_row(sparse_matrix *sm, size_t row_ind, size_t row_desc_beg, size_t row_desc_end, matrix_elem_t *elem);

error add_rows(sparse_matrix *sm, size_t ind_up_to);

error add_matr_elem(sparse_matrix *sm, double val, size_t row_ind, size_t col_ind);

error empty_sp_matrix(sparse_matrix *sm);

void delete_sp_matrix(sparse_matrix *sm);

error init_sp_matrix_from_file(sparse_matrix *sm, FILE *f);

#endif