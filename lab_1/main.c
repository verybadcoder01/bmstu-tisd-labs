#include "stdio.h"
#include "constants.h"
#include "long_division.h"
#include "long_numbers.h"
#include "long_numbers_io.h"

void print_error_table(void) {
    printf("Таблица расшифровки ошибок:\n");
    printf("1 - Неправильный ввод целого числа\n");
    printf("2 - Неправильный ввод числа с плавающей точкой\n");
    printf("3 - Неправильный ввод экспонента в числе с плавающей точкой\n");
    printf("4 - У числа поставлен некорректный знак\n");
    printf("5 - Деление на ноль\n");
    printf("6 - В процессе деления была достигнута машинная бесконечность (переполнение экспоненты)\n");
    printf("7 - В процессе деления был достигнут машинный ноль\n");
}

int main(void)
{
    printf("Пожалуйста, введите целое число\n");
    printf("Формат ввода: +-XXXXXX\n");
    printf("|----|----|----|----|----|----|----|----|\n");
    big_decimal decimal, res;
    big_integer integer;
    error rc = input_big_integer(&integer);
    if (rc){
        printf("Error %d\n", rc);
        print_error_table();
        return rc;
    }
    printf("Пожалуйста, введите число с плавающей точкой и экспонентой\n");
    printf("Формат ввода: +-XXXXXX.YYYYY(E/e)KKK\n");
    printf("|----|----|----|----|----|----|----|----|#|---|\n");
    rc = input_big_decimal(&decimal);
    if (rc){
        printf("Error %d\n", rc);
        print_error_table();
        return rc;
    }
    rc = divide_by_big_decimal(&integer, &decimal, &res);
    if (rc){
        printf("Error %d\n", rc);
        print_error_table();
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
            print_error_table();
            return rc;
        }
    }
    printf("Result: ");
    print_big_decimal(&res);
    return 0;
}
