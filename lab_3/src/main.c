#include "sparse_matr.h"
#include "sparse_vec.h"
#include "perf_test.h"
#include "utils.h"

int main(void)
{
    sparse_matrix *sm = NULL;
    error rc = empty_sp_matrix(&sm);
    if (rc){
        delete_sp_matrix(sm);
        interface_printf_err(rc);
        return rc;
    }
    rc = init_sp_matrix_from_file(sm, stdin);
    if (rc)
    {
        delete_sp_matrix(sm);
        interface_printf_err(rc);
        return rc;
    }

    sparse_vector *sv = NULL;
    rc = empty_sp_vector(&sv);
    if (rc){
        delete_sp_vec(sv);
        interface_printf_err(rc);
        return rc;
    }
    rc = init_sp_vector_from_file(sv, stdin);
    if (rc)
    {
        delete_sp_matrix(sm);
        delete_sp_vec(sv);
        interface_printf_err(rc);
        return rc;
    }
    sparse_vector *res = NULL;
    rc = empty_sp_vector(&res);
    if (rc){
        delete_sp_matrix(sm);
        delete_sp_vec(sv);
        delete_sp_vec(res);
        interface_printf_err(rc);
        return rc;
    }
    rc = mult_mat_vec(sm, sv, res);
    delete_sp_matrix(sm);
    delete_sp_vec(sv);
    if (rc)
    {
        delete_sp_vec(res);
        interface_printf_err(rc);
        return rc;
    }
    print_sp_vec(res);
    delete_sp_vec(res);
    return 0;
}