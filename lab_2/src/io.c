#include "io.h"
#include "ctype.h"
#include "malloc.h"

error scan(FILE *f, char **result, size_t *res_size){
    size_t cap = 32, len = 0;
    char *buf = malloc(cap);
    if (!buf){
        return ALLOC_ERROR;
    }
    int ch;
    while ((ch = fgetc(f)) != EOF && (isspace(ch) || ch == '\n'));
    if (ch == EOF){
        free(buf);
        return EOF;
    }
    buf[len++] = (char)ch;
    while ((ch = fgetc(f)) != EOF && ch != '\n'){
        if (len >= cap - 1){
            cap *= 2;
            char *temp = realloc(buf, cap);
            if (!temp){
                free(buf);
                return ALLOC_ERROR;
            }
            buf = temp;
        }
        buf[len++] = (char)ch;
    }
    buf[len] = '\0';
    if (len + 1 < cap){
        char *temp = realloc(buf, len + 1);
        if (temp){
            buf = temp;
            cap = len + 1;
        }
    }
    *result = buf;
    *res_size = len;
    return 0;
}

void print_err_table(){
    interface_printf("Таблица расшифровки ошибок.\n");
    interface_printf("1 - Ошибка выделения памяти\n");
    interface_printf("2 - Отсутствует элемент для удаления\n");
    interface_printf("3 - Пустой ввод\n");
    interface_printf("4 - Файл недоступен\n");
    interface_printf("5 - Ошибка ввода\n");
    interface_printf("7 - Таблица не была создана\n");
    interface_printf("8 - Неправильный выбор в меню\n");
    interface_printf("9 - Неправильный тип литературы\n");
    interface_printf("10 - Неправильный тип художественной литературы\n");
    interface_printf("11 - Неправильный тип детской литературы\n");
    interface_printf("12 - Неправильный тип технической литературы\n");
}

void interface_printf(const char *msg) {
    (void)msg;
    #ifdef DEBUG_PRINT
    printf(msg);
    #endif    
}

void interface_print_error(error rc) {
    (void)rc;
    #ifdef DEBUG_PRINT
    printf("Ошибка %d\n", rc);
    print_err_table();
    #endif
}