#include "regular_matr.h"
#include "stdlib.h"

void delete_matrix(matrix *matr)
{
    if (!matr)
    {
        return;
    }
    if (matr->data)
    {
        for (int i = 0; i < matr->rows; i++)
        {
            free(matr->data[i]);
        }
        free(matr->data);
    }
    free(matr);
}

error empty_matrix(matrix **matr)
{
    *matr = malloc(sizeof(matrix));
    if (!*matr)
    {
        return ALLOC_ERROR;
    }
    (*matr)->cols = 0;
    (*matr)->rows = 0;
    (*matr)->data = NULL;
    return 0;
}

error init_matrix_from_file(matrix *matr, FILE *f)
{
    if (fscanf(f, "%d %d", &matr->rows, &matr->cols) != 2)
    {
        return INPUT_ERROR;
    }
    if (matr->rows <= 0 || matr->cols <= 0)
    {
        return WRONG_MAT_ELEM_CNT;
    }

    matr->data = (double **)malloc(matr->rows * sizeof(double *));
    if (!matr->data)
    {
        return ALLOC_ERROR;
    }
    for (int i = 0; i < matr->rows; ++i)
    {
        matr->data[i] = NULL;
    }

    for (int i = 0; i < matr->rows; i++)
    {
        matr->data[i] = (double *)malloc(matr->cols * sizeof(double));
        if (!matr->data[i])
        {
            return ALLOC_ERROR;
        }

        for (int j = 0; j < matr->cols; j++)
        {
            if (fscanf(f, "%lf", &matr->data[i][j]) != 1)
            {
                return INPUT_ERROR;
            }
        }
    }
    return 0;
}

error empty_vector(vector **vec)
{
    *vec = malloc(sizeof(vector));
    if (!*vec)
    {
        return ALLOC_ERROR;
    }
    (*vec)->data = NULL;
    (*vec)->elems = 0;
    return 0;
}

void delete_vector(vector *vec)
{
    if (!vec)
    {
        return;
    }
    free(vec->data);
    free(vec);
}

error init_vector_from_file(vector *vec, FILE *f)
{
    if (fscanf(f, "%d", &vec->elems) != 1)
    {
        return INPUT_ERROR;
    }
    if (vec->elems <= 0)
    {
        return WRONG_VEC_ELEM_CNT;
    }
    vec->data = malloc(vec->elems * sizeof(double));
    if (!vec->data)
    {
        return ALLOC_ERROR;
    }
    for (int i = 0; i < vec->elems; ++i)
    {
        if (fscanf(f, "%lf", &vec->data[i]) != 1)
        {
            return INPUT_ERROR;
        }
    }
    return 0;
}

error mult_regular_matr_vec(matrix *m, vector *v, vector *res) {
    if (m->cols != v->elems){
        return INCOMPATIBLE_SIZES;
    }
    res->data = malloc(v->elems * sizeof(double));
    if (!res->data){
        return ALLOC_ERROR;
    }
    res->elems = v->elems;
    for (int i = 0; i < m->rows; ++i){
        for (int j = 0; j < m->cols; ++j){
            res->data[i] += m->data[i][j] * v->data[j];
        }
    }
    return 0;
}
