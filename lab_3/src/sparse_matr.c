#include "sparse_matr.h"
#include "memory.h"
#include "sort.h"
#include "stdlib.h"
#include "utils.h"

void delete_sp_matrix(sparse_matrix *sm)
{
    if (!sm)
    {
        return;
    }
    free(sm->mat_straight);
    free(sm->row_start_inds);
    free(sm);
    sm = NULL;
}

error empty_sp_matrix(sparse_matrix **sm)
{
    delete_sp_matrix(*sm);
    *sm = malloc(sizeof(sparse_matrix));
    if (!*sm)
    {
        return ALLOC_ERROR;
    }
    (*sm)->mat_straight = NULL;
    (*sm)->row_start_inds = NULL;
    (*sm)->rows_cnt = 0;
    (*sm)->rows_cap = 0;
    (*sm)->mat_elems_cnt = 0;
    (*sm)->mat_elems_cap = 0;
    (*sm)->true_cols = 0;
    (*sm)->true_rows = 0;
    return 0;
}

int comp_matr_elems(const void *l, const void *r)
{
    return (int)((matrix_elem_t *)l)->col_ind - (int)((matrix_elem_t *)r)->col_ind;
}

error push_elem_to_row(sparse_matrix *sm, size_t row_ind, size_t row_desc_beg, size_t row_desc_end, matrix_elem_t *elem)
{
    if (sm->mat_elems_cnt >= sm->mat_elems_cap)
    {
        size_t n_cap = 0;
        if (sm->mat_elems_cnt == 0)
        {
            n_cap = 1;
        }
        else
        {
            n_cap = 2 * sm->mat_elems_cnt;
        }
        matrix_elem_t *tmp_buf = realloc(sm->mat_straight, n_cap * sizeof(matrix_elem_t));
        if (!tmp_buf)
        {
            return ALLOC_ERROR;
        }
        sm->mat_straight = tmp_buf;
        sm->mat_elems_cap = n_cap;
    }
    if (sm->mat_elems_cnt - row_desc_end != 0)
    {
        memmove(sm->mat_straight + row_desc_end + 1, sm->mat_straight + row_desc_end, (sm->mat_elems_cnt - row_desc_end) * sizeof(matrix_elem_t));
    }
    sm->mat_straight[row_desc_end] = *elem;
    for (size_t i = row_ind + 1; i < sm->rows_cnt; ++i)
    {
        sm->row_start_inds[i]++;
    }
    sm->mat_elems_cnt++;
    return merge_sort(sm->mat_straight + row_desc_beg, row_desc_end + 1 - row_desc_beg, sizeof(matrix_elem_t), comp_matr_elems);
}

error add_rows(sparse_matrix *sm, size_t ind_up_to)
{
    if (ind_up_to >= sm->rows_cap)
    {
        size_t n_cap = ind_up_to * 2;
        if (n_cap == 0)
        {
            n_cap = 1;
        }
        size_t *tmp_buf = realloc(sm->row_start_inds, n_cap * sizeof(size_t));
        if (!tmp_buf)
        {
            return ALLOC_ERROR;
        }
        sm->row_start_inds = tmp_buf;
        sm->rows_cap = n_cap;
    }
    for (size_t i = sm->rows_cnt; i <= ind_up_to; ++i)
    {
        sm->row_start_inds[i] = sm->mat_elems_cnt;
    }
    sm->rows_cnt = ind_up_to + 1;
    return 0;
}

error add_matr_elem(sparse_matrix *sm, double val, size_t row_ind, size_t col_ind)
{
    size_t row_desc_start = 0, row_desc_end = 0;
    matrix_elem_t temp_elem = {val, col_ind};
    if (row_ind >= sm->rows_cnt)
    {
        error rc = add_rows(sm, row_ind);
        if (rc)
        {
            return rc;
        }
    }
    row_desc_start = sm->row_start_inds[row_ind];
    if (row_ind == sm->rows_cnt - 1)
    {
        row_desc_end = sm->mat_elems_cnt;
    }
    else
    {
        row_desc_end = sm->row_start_inds[row_ind + 1];
    }
    return push_elem_to_row(sm, row_ind, row_desc_start, row_desc_end, &temp_elem);
}

error read_size(sparse_matrix *sm, FILE *f)
{
    interface_printf("Введите настоящее число строк и столбцов матрицы\n");
    int rc = fscanf(f, "%zu %zu", &sm->true_rows, &sm->true_cols);
    if (rc == EOF)
    {
        return rc;
    }
    if (rc != 2)
    {
        return INPUT_ERROR;
    }
    if (sm->true_rows == 0 || sm->true_cols == 0)
    {
        return EMPTY_MAT;
    }
    return 0;
}

error init_sp_matrix_from_file(sparse_matrix *sm, FILE *f)
{
    int rc = read_size(sm, f);
    if (rc)
    {
        return rc;
    }
    interface_printf("Введите число ненулевых элементов матрицы\n");
    int matr_elems = 0;
    if (fscanf(f, "%d", &matr_elems) != 1){
        return INPUT_ERROR;
    }
    if (matr_elems < 0 || (size_t)matr_elems > sm->true_cols * sm->true_rows){
        return WRONG_MAT_ELEM_CNT;
    }
    if (matr_elems == 0){
        return EMPTY_MAT;
    }
    interface_printf("Введите элементы матрицы\n");
    interface_printf("Формат ввода: значение, номер строки, номер столбца (в 0-индексации)\n");
    for (int _ = 0; _ < matr_elems; ++_)
    {
        double val = 0;
        int row = 0, col = 0;
        int rc = fscanf(f, "%lf %d %d", &val, &row, &col);
        if (rc != 3)
        {
            return INPUT_ERROR;
        }
        if (row < 0 || row >= (int)sm->true_rows)
        {
            return WRONG_MAT_ROW;
        }
        if (col < 0 || col >= (int)sm->true_cols)
        {
            return WRONG_MAT_COL;
        }
        rc = add_matr_elem(sm, val, (size_t)row, (size_t)col);
        if (rc)
        {
            return rc;
        }
    }
    return 0;
}