#include "long_numbers_io.h"
#include "stdio.h"
#include "ctype.h"
#include "string.h"

error sign(const char *str, int *res)
{
    if (*str == '-')
    {
        *res = -1;
    }
    else if (*str == '+')
    {
        *res = 1;
    }
    else
    {
        return WRONG_SIGN;
    }
    return 0;
}

error str_to_int(const char *str, int *result)
{
    if (!str || *str == '\0')
    {
        return NOT_A_NUMBER_INPUT;
    }
    int sign = 1, tmp = 0;
    const char *ptr = str;
    if (*ptr == '-')
    {
        sign = -1;
        ptr++;
    }
    else if (*ptr == '+')
    {
        ptr++;
    }
    else if (!isdigit(*ptr))
    {
        return NOT_A_NUMBER_INPUT;
    }
    while (*ptr != '\0')
    {
        if (!isdigit(*ptr))
        {
            return NOT_A_NUMBER_INPUT;
        }
        int digit = *ptr - '0';
        tmp = tmp * 10 + digit;
        ptr++;
    }
    *result = tmp * sign;
    return 0;
}

error input_big_decimal(big_decimal *result)
{
    char tmp[TOTAL_DECIMAL_LENGTH + 4];
    if (fgets(tmp, TOTAL_DECIMAL_LENGTH + 4, stdin) == NULL)
    {
        return WRONG_DECIMAL_INPUT;
    }
    if (!strchr(tmp, '\n') || !strcmp(tmp, "\n"))
    {
        return WRONG_DECIMAL_INPUT;
    }
    tmp[strcspn(tmp, "\n")] = '\0';
    int length = (int)strlen(tmp);

    error rc = sign(tmp, &result->mantis_sign);
    if (rc)
    {
        return rc;
    }

    int i = 1;
    result->before_point_count = 0;
    for (; i < length && result->before_point_count < MAX_MANTISS_LENGTH && tmp[i] != '.'; ++i)
    {
        if (!isdigit(tmp[i]))
        {
            return WRONG_DECIMAL_INPUT;
        }
        result->mantis_before_point[i - 1] = tmp[i] - '0';
        result->before_point_count++;
    }
    if (result->before_point_count == 0)
    {
        return WRONG_DECIMAL_INPUT;
    }
    if (i == length || result->before_point_count > MAX_MANTISS_LENGTH)
    {
        return WRONG_DECIMAL_INPUT;
    }

    i++;
    int ival = i;
    result->after_point_count = 0;
    int not_zero_after_point = 0;
    for (; i < length && result->after_point_count < MAX_MANTISS_LENGTH &&
           tmp[i] != 'E' && tmp[i] != 'e';
         ++i)
    {
        if (!isdigit(tmp[i]))
        {
            return WRONG_DECIMAL_INPUT;
        }
        result->mantis_after_point[i - ival] = tmp[i] - '0';
        if (result->mantis_after_point[i - ival] != 0)
        {
            not_zero_after_point = 1;
        }
        result->after_point_count++;
    }
    if (not_zero_after_point == 0)
    {
        result->after_point_count = 0;
        memset(result->mantis_after_point, 0, sizeof(result->mantis_after_point));
    }
    else
    {
        if (i == length || (result->before_point_count + result->after_point_count == MAX_MANTISS_LENGTH &&
                            (tmp[i] != 'E' && tmp[i] != 'e')))
        {
            return WRONG_DECIMAL_INPUT;
        }

        if (result->after_point_count == 1 && result->mantis_after_point[0] == 0)
        {
            result->after_point_count = 0;
        }
    }
    i++;
    rc = str_to_int(tmp + i, &result->exponent);
    if (!rc)
    {
        if (result->exponent > MAX_EXPONENT_VALUE)
        {
            rc = EXPONENT_OVERFLOW;
        }
        else if (result->exponent < -MAX_EXPONENT_VALUE)
        {
            rc = EXPONENT_UNDERFLOW;
        }
    }
    reverse_array(result->mantis_before_point, result->before_point_count);
    reverse_array(result->mantis_after_point, result->after_point_count);
    return rc;
}

error input_big_integer(big_integer *result)
{
    char tmp[MAX_INTEGER_LENGTH + 3];
    if (fgets(tmp, MAX_INTEGER_LENGTH + 3, stdin) == NULL)
    {
        return WRONG_INTEGER_INPUT;
    }
    if (!strchr(tmp, '\n') || !strcmp(tmp, "\n"))
    {
        return WRONG_INTEGER_INPUT;
    }
    tmp[strcspn(tmp, "\n")] = '\0';
    int length = (int)strlen(tmp);
    if (length - 2 > MAX_INTEGER_LENGTH)
    {
        return WRONG_INTEGER_INPUT;
    }

    error rc = sign(tmp, &result->sign);
    if (rc)
    {
        return rc;
    }
    result->dig_count = 0;
    int i = 1;
    for (; i < length; ++i)
    {
        if (!isdigit(tmp[i]))
        {
            return WRONG_INTEGER_INPUT;
        }
        result->digits[result->dig_count] = tmp[i] - '0';
        result->dig_count++;
    }
    reverse_array(result->digits, result->dig_count);
    return 0;
}

void print_big_decimal(big_decimal *data)
{
    reverse_array(data->mantis_before_point, data->before_point_count);
    reverse_array(data->mantis_after_point, data->after_point_count);
    if (data->mantis_sign == -1)
    {
        printf("-");
    }
    else
    {
        printf("+");
    }
    for (int i = 0; i < data->before_point_count; ++i)
    {
        printf("%d", data->mantis_before_point[i]);
    }
    if (data->before_point_count == 0)
    {
        printf("0");
    }
    printf(".");
    for (int i = 0; i < data->after_point_count; ++i)
    {
        printf("%d", data->mantis_after_point[i]);
    }
    if (data->after_point_count == 0)
    {
        printf("0");
    }
    printf("E%d\n", data->exponent);
    reverse_array(data->mantis_before_point, data->before_point_count);
    reverse_array(data->mantis_after_point, data->after_point_count);
}
