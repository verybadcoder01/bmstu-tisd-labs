#ifndef REGULAR_MATR_H__
#define REGULAR_MATR_H__

#include "stdio.h"
#include "errors.h"

typedef struct
{
    double **data;
    int rows, cols;
} matrix;

typedef struct
{
    double *data;
    int elems;
} vector;

void delete_matrix(matrix *matr);

error empty_matrix(matrix **matr);

error init_matrix_from_file(matrix *matr, FILE *f);

error empty_vector(vector **vec);

void delete_vector(vector *vec);

error init_vector_from_file(vector *vec, FILE *f);

error mult_regular_matr_vec(matrix *m, vector *v, vector *res);

#endif