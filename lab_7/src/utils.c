#include "hash_table.h"
#define _GNU_SOURCE
#include "errors.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "tree.h"
#include "utils.h"

void print_menu_struct()
{
    printf("Выберите структуру данных:\n");
    printf("1. AVL-дерево\n");
    printf("2. Хэш-таблица с открытой адресацией\n");
    printf("3. Хэш-таблица с методом цепочек\n");
}

void print_menu_tree()
{
    printf("Выберите действие над деревом:\n");
    printf("1. Построить на числах из файла\n");
    printf("2. Добавление числа\n");
    printf("3. Поиск числа\n");
    printf("4. Удаление числа\n");
    printf("5. Вывод 3 обходов дерева\n");
    printf("6. Вывод дерева в graphviz\n");
    printf("0. Выход\n");
}

void print_menu_table()
{
    printf("Выберите действие над таблицей:\n");
    printf("1. Построить на числах из файла\n");
    printf("2. Добавление числа\n");
    printf("3. Поиск числа\n");
    printf("4. Удаление числа\n");
    printf("5. Вывод таблицы\n");
    printf("0. Выход\n");
}

void scan_newlines(FILE *f)
{
    int c;
    while ((c = fgetc(f)) != '\n' && c != EOF)
        ;
}

int count_nums_in_file(FILE *f)
{
    int dummy = 0;
    int cnt = 0;
    while (fscanf(f, "%d", &dummy) == 1)
    {
        cnt++;
    }
    if (!feof(f))
    {
        rewind(f);
        return -1;
    }
    rewind(f);
    return cnt;
}

error add_nums_from_file_to_tree(node_t **root, FILE *f)
{
    error rc = 0;
    int num = 0;
    while (fscanf(f, "%d", &num) == 1)
    {
        rc = tree_insert(root, comp_node_data, num);
        if (rc && rc != DUPLICATE_KEY)
        {
            break;
        }
        else if (rc == DUPLICATE_KEY)
        {
            printf("В файле обнаружен дубликат ключа\n");
        }
    }
    return rc;
}

error add_nums_from_file_to_ht_open(hash_table_open *ht, FILE *f)
{
    int num = 0;
    error rc = 0;
    while (fscanf(f, "%d", &num) == 1)
    {
        rc = ht_open_insert(ht, num);
        if (rc && rc != DUPLICATE_KEY)
        {
            break;
        }
        else if (rc == DUPLICATE_KEY)
        {
            printf("В файле обнаружен дубликат ключа\n");
        }
    }
    return rc;
}

error add_nums_from_file_to_ht_chain(hash_table_chain *ht, FILE *f)
{
    int num = 0;
    error rc = 0;
    while (fscanf(f, "%d", &num) == 1)
    {
        rc = ht_chain_insert(ht, num);
        if (rc && rc != DUPLICATE_KEY)
        {
            break;
        }
        else if (rc == DUPLICATE_KEY)
        {
            printf("В файле обнаружен дубликат ключа\n");
        }
    }
    return rc;
}

error read_and_open_file(FILE **f, const char *mode)
{
    char *filename = NULL;
    size_t dummy = 0;
    printf("Введите имя файла:\n");
    if (getline(&filename, &dummy, stdin) == -1)
    {
        free(filename);
        printf("Ошибка %d\n", INPUT_ERROR);
        return INPUT_ERROR;
    }
    if (!strchr(filename, '\n') || strcmp(filename, "\n") == 0)
    {
        free(filename);
        printf("Ошибка %d\n", INPUT_ERROR);
        return INPUT_ERROR;
    }
    filename[strcspn(filename, "\n")] = '\0';
    *f = fopen(filename, mode);
    free(filename);
    if (!*f)
    {
        printf("Ошибка %d\n", WRONG_INPUT_FILE);
        return WRONG_INPUT_FILE;
    }
    return 0;
}
