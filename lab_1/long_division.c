#include "long_division.h"
#include "string.h"
#include "constants.h"

error divide_by_big_decimal(const big_integer *a, const big_decimal *b, big_decimal *result) {
    if (is_zero_decimal(b)){
        return DIVISION_BY_ZERO;
    }
    if (is_zero_integer(a)){
        init_big_decimal_zero(result);
        return 0;
    }
    big_integer current_divident, a_copy = *a;
    a_copy.sign = 1;
    big_decimal b_copy = *b;
    b_copy.mantis_sign = 1;
    init_big_decimal_zero(result);
    error rc = shift_left(&b_copy, b_copy.after_point_count);
    if (rc){
        return rc;
    }
    result->exponent = -b_copy.exponent;
    b_copy.exponent = 0;
    big_integer part_divide;
    int dig_ind = -10, write_before_point = 1;
    for (int i = a_copy.dig_count - 1; i >= 0; --i){
        part_divide.sign = a_copy.sign;
        add_digit_to_end(part_divide.digits, part_divide.dig_count, a_copy.digits[i]);
        part_divide.dig_count++;
        if (compare_integer_decimal(&part_divide, &b_copy) >= 0){
            current_divident = part_divide;
            dig_ind = i - 1;
            break;
        }
    }
    if (dig_ind == -10){
        current_divident = a_copy;
    }
    big_integer b_integer;
    memcpy(b_integer.digits, b_copy.mantis_before_point, sizeof(b_copy.mantis_before_point));
    memcpy(b_integer.digits + b_copy.before_point_count, b_copy.mantis_after_point, sizeof(b_copy.mantis_after_point));
    b_integer.sign = 1;
    b_integer.dig_count = b_copy.before_point_count + b_copy.after_point_count;
    while (1){
        int i = 0;
        for (; i <= 9; ++i){
            big_integer cur_mult_res;
            cur_mult_res.dig_count = 0;
            memset(cur_mult_res.digits, 0, sizeof(cur_mult_res.digits));
            mult_big_integer_digit(&b_integer, i, &cur_mult_res);
            if (compare_integer_integer(&cur_mult_res, &current_divident) > 0){
                i--;
                break;
            }
        }
        if (i == 10){
            i--;
        }
        big_integer sub_from_divident;
        memset(sub_from_divident.digits, 0, sizeof(sub_from_divident.digits));
        sub_from_divident.dig_count = 0;
        sub_from_divident.sign = 1;
        mult_big_integer_digit(&b_integer, i, &sub_from_divident);
        if (!write_before_point){
            rc = leading_zeros_to_exponent(result);
            if (rc){
                return rc;
            }
        }
        if (result->before_point_count + result->after_point_count == MAX_MANTISS_LENGTH){
            rc = round_big_decimal(result, i);
            if (rc){
                return rc;
            }
            break;
        }
        if (write_before_point){
            add_digit_to_end(result->mantis_before_point, result->before_point_count, i);
            result->before_point_count++;
        } else {
            add_digit_to_end(result->mantis_after_point, result->after_point_count, i);
            result->after_point_count++;
        }
        big_integer tmp;
        subtract_big_integers(&current_divident, &sub_from_divident, &tmp);
        current_divident = tmp;
        if (dig_ind < 0){
            add_digit_to_end(current_divident.digits, current_divident.dig_count, 0);
            write_before_point = 0;
        } else
        {
            add_digit_to_end(current_divident.digits, current_divident.dig_count, a_copy.digits[dig_ind]);
            dig_ind--;
        }
        current_divident.dig_count++;
        while (is_zero_integer(&current_divident)){
            if (dig_ind < 0){
                normalize(result);
                return 0;
            }
            add_digit_to_end(current_divident.digits, current_divident.dig_count, a_copy.digits[dig_ind]);
            dig_ind--;
            current_divident.dig_count++;
            result->exponent++;
            if (result->exponent > MAX_EXPONENT_VALUE){
                return EXPONENT_OVERFLOW;
            }
        }
    }
    normalize(result);
    return 0;
}
