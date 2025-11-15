#include "errors.h"
#include "simulation.h"
#include "stdio.h"

int main(void)
{
    printf("Выберите режим работы программы:\n");
    printf("1 - С выводом освобожденных адресов\n");
    printf("2 - С замером времени\n");
    mode m;
    if (scanf("%d", (int *)&m) != 1)
    {
        return INPUT_ERROR;
        printf("Ошибка ввода\n");
    }
    if (m != USER && m != PERF)
    {
        return WRONG_MODE;
        printf("Неправильный тип, допустимо только 1 и 2\n");
    }
    printf("Выберите тип очереди программы:\n");
    printf("1 - Список\n");
    printf("2 - Статический массив\n");
    queue_type t;
    if (scanf("%d", (int *)&t) != 1)
    {
        return INPUT_ERROR;
        printf("Ошибка ввода\n");
    }
    if (t != LIST && t != ARRAY)
    {
        return WRONG_QUEUE_TYPE;
        printf("Неправильный тип, допустимо только 1 и 2\n");
    }
    error rc = simulate(m, t);
    if (rc == QUEUE_LIST_OVERFLOW || rc == QUEUE_ARR_OVERFLOW)
    {
        printf("Переполнение очереди\n");
    }
    else if (rc)
    {
        printf("Ошибка %d\n", rc);
        printf("Неправильный ввод интервала\n");
        return rc;
    }
    return 0;
}