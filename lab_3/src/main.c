#include "sparse_matr.h"
#include "sparse_vec.h"
#include "perf_test.h"
#include "utils.h"
#include "string.h"

int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "p")){
        printf("starting perf test\n");
        error rc = run_perf_test();
        if (rc){
            interface_printf_err(rc);
            return rc;
        }
        return 0;
    }
    printf("not starting perf test\n");
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
    rc = mult_sp_mat_sp_vec(sm, sv, res);
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