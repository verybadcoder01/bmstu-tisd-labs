#include "utils.h"
#include "errors.h"
#include "stdio.h"

void interface_printf(const char *msg){
    (void)msg;
    #ifdef DEBUG_PRINT
    printf(msg);
    #endif
}

void print_err_table(){
    interface_printf("1 - ошибка выделения памяти\n");
    interface_printf("2 - пустой вектор\n");
    interface_printf("3 - ошибка ввода (например, строка вместо числа)\n");
    interface_printf("4 - неправильный индекс элемента в векторе\n");
    interface_printf("5 - пустая матрица\n");
    interface_printf("6 - неправильный индекс строки в матрице\n");
    interface_printf("7 - неправильный индекс столбца в матрице\n");
    interface_printf("8 - несовместимые размеры для умножения\n");
    interface_printf("9 - недопустимое количество элементов в матрице\n");
    interface_printf("10 - недопустимое количество элементов в векторе\n");
}

void interface_printf_err(error rc) {
    (void)rc;
    #ifdef DEBUG_PRINT
    printf("Ошибка %d\n", rc);
    #endif
}