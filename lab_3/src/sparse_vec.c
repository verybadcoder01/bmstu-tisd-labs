#include "sparse_vec.h"
#include "stdlib.h"
#include "sort.h"
#include "utils.h"

error add_vec_elem(sparse_vector *sv, vec_elem_t *val)
{
    if (sv->elem_cnt < sv->elem_cap)
    {
        sv->vec[sv->elem_cnt] = *val;
        sv->elem_cnt++;
    }
    else
    {
        size_t n_cap = 0;
        if (sv->elem_cnt == 0)
        {
            n_cap = 1;
        }
        else
        {
            n_cap = 2 * sv->elem_cnt;
        }
        vec_elem_t *tmp_buf = realloc(sv->vec, n_cap * sizeof(vec_elem_t));
        if (!tmp_buf)
        {
            return ALLOC_ERROR;
        }
        sv->vec = tmp_buf;
        sv->vec[sv->elem_cnt] = *val;
        sv->elem_cap = n_cap;
        sv->elem_cnt++;
    }
    return 0;
}

error empty_sp_vector(sparse_vector **sv)
{
    delete_sp_vec(*sv);
    *sv = malloc(sizeof(sparse_vector));
    if (!*sv)
    {
        return ALLOC_ERROR;
    }
    (*sv)->elem_cap = 0;
    (*sv)->elem_cnt = 0;
    (*sv)->vec = NULL;
    (*sv)->true_len = 0;
    return 0;
}

void delete_sp_vec(sparse_vector *sv)
{
    if (!sv)
    {
        return;
    }
    free(sv->vec);
    free(sv);
    sv = NULL;
}

int comp_vec_elems(const void *l, const void *r)
{
    return (int)((vec_elem_t *)l)->ind - (int)((vec_elem_t *)r)->ind;
}

error init_sp_vector_from_file(sparse_vector *sv, FILE *f)
{
    interface_printf("Введите вектор\n");
    interface_printf("Введите настоящую длину вектора\n");
    int rc = fscanf(f, "%zu", &sv->true_len);
    if (rc == EOF){
        return EMPTY_VEC;
    }
    if (rc != 1){
        return INPUT_ERROR;
    }
    if (sv->true_len == 0){
        return EMPTY_VEC;
    }
    int vec_elem_cnt = 0;
    interface_printf("Введите число ненулевых элементов вектора\n");
    if (fscanf(f, "%d", &vec_elem_cnt) != 1){
        return INPUT_ERROR;
    }
    if (vec_elem_cnt <= 0 || (size_t)vec_elem_cnt > sv->true_len){
        return WRONG_VEC_ELEM_CNT;
    }
    if (vec_elem_cnt == 0){
        return EMPTY_VEC;
    }
    interface_printf("Введите элементы вектора\n");
    interface_printf("Формат ввода: каждый элемент вектора описывается числом с плавающей точкой (значение) и его индексом (в 0-нумерации)\n");
    double temp_val = 0;
    int temp_coord = 0;
    for (int _ = 0; _ < vec_elem_cnt; ++_)
    {
        rc = fscanf(f, "%lf %d", &temp_val, &temp_coord);
        if (rc != 2)
        {
            return INPUT_ERROR;
        }
        if (temp_coord < 0 || temp_coord >= (int)sv->true_len)
        {
            return WRONG_VEC_COORD;
        }
        vec_elem_t temp_elem = {temp_val, (size_t)temp_coord};
        rc = add_vec_elem(sv, &temp_elem);
        if (rc)
        {
            return rc;
        }
    }
    return merge_sort(sv->vec, sv->elem_cnt, sizeof(vec_elem_t), comp_vec_elems);
}

void print_sp_vec(sparse_vector *sv) {
    interface_printf("Вывод в формате\n| индекс | значение |\n");
    for (size_t i = 0; i < sv->elem_cnt; ++i){
        printf("| %zu | %.6lf |\n", sv->vec[i].ind, sv->vec[i].data);
    }
}