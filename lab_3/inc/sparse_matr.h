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
    size_t mat_elems_cnt;
    size_t *row_start_inds;
    size_t rows_cnt;
    size_t true_rows, true_cols;
} sparse_matrix;

typedef struct {
    double value;
    size_t row, col;
} coord_elem_t;

int comp_matr_elems(const void *l, const void *r);

int comp_coord_elems(const void *l, const void *r);

error empty_sp_matrix(sparse_matrix **sm);

void delete_sp_matrix(sparse_matrix *sm);

error read_size(sparse_matrix *sm, FILE *f);

error init_sp_matrix_from_file(sparse_matrix *sm, FILE *f);

#endif