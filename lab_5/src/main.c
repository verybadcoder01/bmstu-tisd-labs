#include "simulation.h"
#include "stdio.h"
#include "errors.h"

int main(void){
    printf("Выберите режим работы программы:\n");
    printf("1 - С выводом дополнительной информации\n");
    printf("2 - Только с замером времени\n");
    mode m;
    if (scanf("%d", (int*)&m) != 1){
        return INPUT_ERROR;
    }
    if (m != USER && m != PERF){
        return WRONG_MODE;
    }
    printf("Выберите тип очереди программы:\n");
    printf("1 - Список\n");
    printf("2 - Статический массив\n");
    queue_type t;
    if (scanf("%d", (int*)&t) != 1){
        return INPUT_ERROR;
    }
    if (t != LIST && t != ARRAY){
        return WRONG_QUEUE_TYPE;
    }
    error rc = simulate(m, t);
    if (rc){
        printf("%d\n", rc);
    }
    return 0;
}