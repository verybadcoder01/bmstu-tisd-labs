#include "stdio.h"
#include "constants.h"
#include "long_division.h"
#include "long_numbers.h"

// TODO: дописать тесты, сделать проверки на ввод, докинуть проверки на макс экспоненту

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
        shift_right(&res, res.before_point_count);
    }
    printf("Result is: ");
    print_big_decimal(&res);
    return 0;
}
