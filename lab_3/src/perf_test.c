#include "perf_test.h"
#include "math.h"

int bin_search_ind(size_t target, vec_elem_t *elems, size_t size) {
    int l = 0, r = (int)size;
    while (r - l > 1){
        int mid = (l + r) / 2;
        if (elems[mid].ind > target){
            r = mid;
        } else {
            l = mid;
        }
    }
    if (elems[l].ind == target){
        return l;
    } else if (r < (int)size && elems[r].ind == target){
        return r;
    }
    return -1;
}

error mult_mat_vec(sparse_matrix *sm, sparse_vector *sv, sparse_vector *res) {
    if (sm->true_cols != sv->true_len){
        return INCOMPATIBLE_SIZES;
    }
    res->true_len = sv->true_len;
    for (size_t i = 0; i < sm->rows_cnt; ++i){
        size_t row_desc_beg = sm->row_start_inds[i];
        size_t row_desc_end = sm->mat_elems_cnt;
        if (i < sm->rows_cnt - 1){
            row_desc_end = sm->row_start_inds[i + 1];
        }
        double cur_data = 0;
        for (size_t j = row_desc_beg; j < row_desc_end; ++j){
            int ind = bin_search_ind(sm->mat_straight[j].col_ind, sv->vec, sv->elem_cnt);
            if (ind != -1){
                cur_data += sm->mat_straight[j].data * sv->vec[ind].data;
            }
        }
        if (fabs(cur_data) > 1e-6){
            vec_elem_t n_elem = {cur_data, i};
            error rc = add_vec_elem(res, &n_elem);
            if (rc){
                return rc;
            }
        }
    }
    return 0;
}
