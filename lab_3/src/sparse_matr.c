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
    (*sm)->mat_elems_cnt = 0;
    (*sm)->true_cols = 0;
    (*sm)->true_rows = 0;
    return 0;
}

int comp_matr_elems(const void *l, const void *r)
{
    return (int)((matrix_elem_t *)l)->col_ind - (int)((matrix_elem_t *)r)->col_ind;
}

int comp_coord_elems(const void *l, const void *r)
{
    const coord_elem_t *el = (const coord_elem_t *)l;
    const coord_elem_t *er = (const coord_elem_t *)r;
    if (el->row != er->row)
    {
        return (int)(el->row - er->row);
    }
    return (int)(el->col - er->col);
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

error build_sparse_matr(sparse_matrix *sm, coord_elem_t *elems)
{
    error rc = merge_sort(elems, sm->mat_elems_cnt, sizeof(coord_elem_t), comp_coord_elems);
    if (rc)
    {
        return rc;
    }
    rc = check_duplicates(elems, sm->mat_elems_cnt, sizeof(coord_elem_t), comp_coord_elems);
    if (rc)
    {
        return rc;
    }
    sm->mat_straight = malloc(sm->mat_elems_cnt * sizeof(matrix_elem_t));
    if (!sm->mat_straight)
    {
        return ALLOC_ERROR;
    }
    sm->row_start_inds = malloc((sm->true_rows + 1) * sizeof(size_t));
    if (!sm->row_start_inds)
    {
        return ALLOC_ERROR;
    }
    for (size_t i = 0; i <= sm->true_rows; ++i)
    {
        sm->row_start_inds[i] = 0;
    }
    size_t current_index = 0;
    for (size_t row = 0; row < sm->true_rows; ++row)
    {
        sm->row_start_inds[row] = current_index;

        while (current_index < sm->mat_elems_cnt && elems[current_index].row == row)
        {
            sm->mat_straight[current_index].data = elems[current_index].value;
            sm->mat_straight[current_index].col_ind = elems[current_index].col;
            current_index++;
        }
    }
    sm->row_start_inds[sm->true_rows] = sm->mat_elems_cnt;
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
    if (fscanf(f, "%d", &matr_elems) != 1)
    {
        return INPUT_ERROR;
    }
    if (matr_elems < 0 || (size_t)matr_elems > sm->true_cols * sm->true_rows)
    {
        return WRONG_MAT_ELEM_CNT;
    }
    if (matr_elems == 0)
    {
        return EMPTY_MAT;
    }
    sm->mat_elems_cnt = matr_elems;
    sm->rows_cnt = sm->true_rows;
    interface_printf("Введите элементы матрицы\n");
    interface_printf("Формат ввода: значение, номер строки, номер столбца (в 0-индексации)\n");
    coord_elem_t *matr_data = malloc(matr_elems * sizeof(coord_elem_t));
    if (!matr_data)
    {
        return ALLOC_ERROR;
    }
    for (int i = 0; i < matr_elems; ++i)
    {
        double val = 0;
        int row = 0, col = 0;
        int rc = fscanf(f, "%lf %d %d", &val, &row, &col);
        if (rc != 3)
        {
            free(matr_data);
            return INPUT_ERROR;
        }
        if (row < 0 || row >= (int)sm->true_rows)
        {
            free(matr_data);
            return WRONG_MAT_ROW;
        }
        if (col < 0 || col >= (int)sm->true_cols)
        {
            free(matr_data);
            return WRONG_MAT_COL;
        }
        coord_elem_t cur = {val, row, col};
        matr_data[i] = cur;
    }
    rc = build_sparse_matr(sm, matr_data);
    free(matr_data);
    return rc;
}