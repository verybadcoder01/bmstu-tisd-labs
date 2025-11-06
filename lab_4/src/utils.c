#include "utils.h"
#include "stdio.h"

void interface_printf_ptr(const void* ptr) {
    (void)ptr;
    #ifdef DEBUG_PRINT
    printf("%p\n", ptr);
    #endif
}

void interface_printf(const char *msg) {
    (void)msg;
    #ifdef DEBUG_PRINT
    printf("%s", msg);
    #endif
}

void print_err_table() {
    interface_printf("1 - Ошибка выделения памяти\n");
    interface_printf("2 - Ошибка ввода\n");
    interface_printf("3 - Переполнение стека\n");
    interface_printf("4 - Удаление из пустого стека\n");
    interface_printf("5 - Выбран неправильный тип стека\n");
    interface_printf("6 - Выбран неправильный пункт меню\n");
    interface_printf("7 - Ошибка запуска сравнения производительности\n");
}

void interface_printf_err(error rc) {
    (void)rc;
    #ifdef DEBUG_PRINT
    printf("Ошибка %d\n", rc);
    print_err_table();
    #endif
}

void interface_printf_menu() {
    interface_printf("1 - Добавить элемент наверх стека\n");
    interface_printf("2 - Удалить элемент сверху стека\n");
    interface_printf("3 - Вывести элементы стека\n");
    interface_printf("4 - Вывести стек в обратном порядке, с развернутыми элементами\n");
    interface_printf("5 - Завершить\n");
}

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}