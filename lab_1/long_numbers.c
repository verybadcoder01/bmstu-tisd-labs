#include "long_numbers.h"
#include "string.h"
#include "stdio.h"

void init_big_decimal_zero(big_decimal *num)
{
    memset(num->mantis_before_point, 0, sizeof(num->mantis_before_point));
    memset(num->mantis_after_point, 0, sizeof(num->mantis_after_point));
    num->before_point_count = 0;
    num->after_point_count = 0;
    num->exponent = 0;
    num->mantis_sign = 1;
}

int is_zero_decimal(const big_decimal *num)
{
    if (num->before_point_count == 0 || num->mantis_before_point[num->before_point_count - 1] == 0)
    {
        if (num->after_point_count == 0 || (num->after_point_count == 1 && num->mantis_after_point[0] == 0))
        {
            return 1;
        }
    }
    return 0;
}

int is_zero_integer(const big_integer *num)
{
    if (num->dig_count == 0 || num->digits[num->dig_count - 1] == 0)
    {
        return 1;
    }
    return 0;
}

void add_digit_to_end(char *num, int size, char digit)
{
    memmove(num + 1, num, size);
    num[0] = digit;
}

void normalize(big_decimal *num)
{
    reverse_array(num->mantis_before_point, num->before_point_count);
    reverse_array(num->mantis_after_point, num->after_point_count);
    int leading_zeros = 0;
    while (leading_zeros < num->before_point_count - 1 &&
           num->mantis_before_point[leading_zeros] == 0)
    {
        leading_zeros++;
    }

    if (leading_zeros > 0)
    {
        for (int i = 0 ; i < num->before_point_count - leading_zeros ; i++)
        {
            num->mantis_before_point[i] = num->mantis_before_point[i + leading_zeros];
        }
        num->before_point_count -= leading_zeros;
    }

    while (num->after_point_count > 0 &&
           num->mantis_after_point[num->after_point_count - 1] == 0)
    {
        num->after_point_count--;
    }
    if (num->before_point_count == 1 && num->mantis_before_point[0] == 0 &&
        num->after_point_count == 0)
    {
        num->exponent = 0;
        num->mantis_sign = 1;
    }
    reverse_array(num->mantis_before_point, num->before_point_count);
    reverse_array(num->mantis_after_point, num->after_point_count);
}

error leading_zeros_to_exponent(big_decimal *num)
{
    int leading_zeros = 0, i = num->after_point_count - 1;
    while (i >= 0 && num->mantis_after_point[i] == 0)
    {
        leading_zeros++;
        i--;
    }
    num->after_point_count -= leading_zeros;
    num->exponent -= leading_zeros;
    if (num->exponent < -MAX_EXPONENT_VALUE)
    {
        return EXPONENT_UNDERFLOW;
    }
    return 0;
}

error round_big_decimal(big_decimal *num, int new_digit)
{
    int carry = (new_digit >= 5) ? 1 : 0;
    if (carry == 0)
    {
        return 0;
    }
    for (int i = 0 ; i < num->after_point_count ; i++)
    {
        num->mantis_after_point[i] += carry;
        carry = num->mantis_after_point[i] / 10;
        num->mantis_after_point[i] %= 10;
        if (carry == 0)
        {
            break;
        }
    }

    if (carry)
    {
        for (int i = 0 ; i < num->before_point_count ; i++)
        {
            num->mantis_before_point[i] += carry;
            carry = num->mantis_before_point[i] / 10;
            num->mantis_before_point[i] %= 10;
            if (carry == 0)
            {
                break;
            }
        }
    }

    if (carry)
    {
        num->exponent++;
        if (num->exponent > MAX_EXPONENT_VALUE)
        {
            return EXPONENT_OVERFLOW;
        }
        memset(num->mantis_before_point, 0, sizeof(num->mantis_before_point));
        num->mantis_before_point[0] = 1;
        num->before_point_count = 1;
        memset(num->mantis_after_point, 0, sizeof(num->mantis_after_point));
        num->after_point_count = 1;
    }
    return 0;
}

error shift_right(big_decimal *num, int places)
{
    if (places <= 0) return 0;

    int available_places = MAX_MANTISS_LENGTH - num->after_point_count;
    if (available_places <= 0) return 0;

    if (places > available_places)
    {
        places = available_places;
    }

    memcpy(num->mantis_after_point + num->after_point_count, num->mantis_before_point + num->before_point_count - places, places);
    memset(num->mantis_before_point + num->before_point_count - places, 0, places);
    num->before_point_count -= places;
    num->after_point_count += places;
    num->exponent += places;
    if (num->exponent > MAX_EXPONENT_VALUE)
    {
        return EXPONENT_OVERFLOW;
    }

    return 0;
}

error shift_left(big_decimal *num, int places)
{
    if (places <= 0) return 0;

    if (num->after_point_count > 0)
    {
        if (num->before_point_count + places > MAX_MANTISS_LENGTH)
        {
            places = MAX_MANTISS_LENGTH - num->before_point_count;
            if (places <= 0)
            {
                return 0;
            }
        }
        for (int i = num->before_point_count - 1 ; i >= 0 ; i--)
        {
            num->mantis_before_point[i + places] = num->mantis_before_point[i];
        }
        for (int i = 0 ; i < places ; i++)
        {
            if (i < num->after_point_count)
            {
                num->mantis_before_point[i] = num->mantis_after_point[i];
            } else
            {
                num->mantis_before_point[i] = 0;
            }
        }
        for (int i = 0 ; i < num->after_point_count - places ; i++)
        {
            num->mantis_after_point[i] = num->mantis_after_point[i + places];
        }

        num->before_point_count += places;
        num->after_point_count = (num->after_point_count > places) ?
                                 num->after_point_count - places : 0;
    } else
    {
        if (num->before_point_count + places > MAX_MANTISS_LENGTH)
        {
            places = MAX_MANTISS_LENGTH - num->before_point_count;
            if (places <= 0)
            {
                return 0;
            }
        }
        for (int i = num->before_point_count - 1 ; i >= 0 ; i--)
        {
            num->mantis_before_point[i + places] = num->mantis_before_point[i];
        }
        for (int i = 0 ; i < places ; i++)
        {
            num->mantis_before_point[i] = 0;
        }

        num->before_point_count += places;
    }

    num->exponent -= places;
    if (num->exponent < -MAX_EXPONENT_VALUE)
    {
        return EXPONENT_UNDERFLOW;
    }
    return 0;
}

void reverse_array(char *arr, int size)
{
    for (int i = 0 ; i < size / 2 ; ++i)
    {
        char tmp = arr[i];
        arr[i] = arr[size - i - 1];
        arr[size - i - 1] = tmp;
    }
}

// Non-negative, a >= b
void subtract_big_integers(const big_integer *a, const big_integer *b, big_integer *result)
{
    char borrow = 0;
    int i;
    result->dig_count = a->dig_count;
    result->sign = 1;

    for (i = 0 ; i < a->dig_count ; i++)
    {
        char digit_a = a->digits[i];
        char digit_b = (i < b->dig_count) ? b->digits[i] : 0;
        char diff = digit_a - digit_b - borrow;

        if (diff < 0)
        {
            diff += 10;
            borrow = 1;
        } else
        {
            borrow = 0;
        }
        result->digits[i] = diff;
    }
    while (result->dig_count > 1 && result->digits[result->dig_count - 1] == 0)
    {
        result->dig_count--;
    }
    if (result->dig_count == 1 && result->digits[0] == 0)
    {
        result->sign = 1;
    }
    if (is_zero_integer(result))
    {
        result->dig_count = 0;
    }
}

void mult_big_integer_digit(const big_integer *a, int digit, big_integer *result)
{
    if (digit == 0 || is_zero_integer(a))
    {
        result->dig_count = 1;
        result->digits[0] = 0;
        result->sign = 1;
        return;
    }
    result->sign = 1;
    int carry = 0;
    int i;
    for (i = 0 ; i < a->dig_count ; i++)
    {
        int product = a->digits[i] * digit + carry;
        result->digits[i] = product % 10;
        carry = product / 10;
    }
    if (carry > 0)
    {
        if (a->dig_count < MAX_INTEGER_LENGTH + 1)
        {
            result->digits[i] = carry;
            result->dig_count = a->dig_count + 1;
        } else
        {
            result->dig_count = a->dig_count;
        }
    } else
    {
        result->dig_count = a->dig_count;
    }
}

// 1 for a > b, -1 for b > a, 0 for equal
int compare_integer_decimal(const big_integer *a, const big_decimal *b)
{
    if (a->sign > 0 && b->mantis_sign < 0)
    {
        return 1;
    } else if (b->mantis_sign > 0 && a->sign < 0)
    {
        return -1;
    }
    int integer_dec_length = b->before_point_count + b->exponent;
    int mode = 1;
    if (a->sign < 0 && b->mantis_sign < 0)
    {
        mode = -1;
    }
    if (integer_dec_length > a->dig_count ||
        (integer_dec_length == a->dig_count && (b->after_point_count - b->exponent) > 0))
    {
        return -1 * mode;
    }
    if (integer_dec_length < a->dig_count)
    {
        return 1 * mode;
    }
    char tmp_digs_array[MAX_MANTISS_LENGTH], tmp_a_digs_array[MAX_INTEGER_LENGTH + 1];
    memcpy(tmp_a_digs_array, a->digits, sizeof(a->digits));
    reverse_array(tmp_a_digs_array, a->dig_count);
    memcpy(tmp_digs_array, b->mantis_before_point, b->before_point_count);
    reverse_array(tmp_digs_array, b->before_point_count);
    memcpy(tmp_digs_array + b->before_point_count, b->mantis_after_point, b->after_point_count);
    reverse_array(tmp_digs_array + b->before_point_count, b->after_point_count);
    for (int i = 0 ; i < a->dig_count ; ++i)
    {
        if (tmp_digs_array[i] > a->digits[i])
        {
            return -1 * mode;
        } else if (a->digits[i] > tmp_digs_array[i])
        {
            return 1 * mode;
        }
    }
    return 0;
}

// 1 for a > b, -1 for b > a, 0 for equal
int compare_integer_integer(const big_integer *a, const big_integer *b)
{
    if (a->sign > 0 && b->sign < 0)
    {
        return 1;
    } else if (a->sign < 0 && b->sign > 0)
    {
        return -1;
    }
    int mode = 1;
    if (a->sign < 0 && b->sign < 0)
    {
        mode = -1;
    }
    if (a->dig_count > b->dig_count)
    {
        return 1 * mode;
    } else if (a->dig_count < b->dig_count)
    {
        return -1 * mode;
    }
    for (int i = a->dig_count - 1 ; i >= 0 ; --i)
    {
        if (a->digits[i] > b->digits[i])
        {
            return 1 * mode;
        } else if (a->digits[i] < b->digits[i])
        {
            return -1 * mode;
        }
    }
    return 0;
}
