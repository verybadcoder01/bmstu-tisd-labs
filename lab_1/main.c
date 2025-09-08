#include "stdio.h"
#include "constants.h"
#include "long_division.h"
#include "long_numbers.h"
#include "long_numbers_io.h"

// TODO: дописать тесты, сделать проверки на ввод

int main(void)
{
    big_decimal decimal, res;
    big_integer integer;
    error rc = input_big_integer(&integer);
    if (rc){
        printf("Error %d\n", rc);
        return rc;
    }
    rc = input_big_decimal(&decimal);
    if (rc){
        printf("Error %d\n", rc);
        return rc;
    }
    rc = divide_by_big_decimal(&integer, &decimal, &res);
    if (rc){
        printf("Error %d\n", rc);
        return rc;
    }
    res.mantis_sign = integer.sign * decimal.mantis_sign;
    if (res.before_point_count == 1 && res.mantis_before_point[0] == 0){
        res.before_point_count = 0;
    }
    if (res.before_point_count > 0){
        rc = shift_right(&res, res.before_point_count);
        if (rc){
            printf("Error %d\n", rc);
            return rc;
        }
    }
    printf("Result: ");
    print_big_decimal(&res);
    return 0;
}
