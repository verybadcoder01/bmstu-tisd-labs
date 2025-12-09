#include "perf_test.h"
#define _GNU_SOURCE
#include "errors.h"
#include "node.h"
#include "stdlib.h"
#include "string.h"
#include "tree.h"
#include <stdio.h>

void scan_newlines(FILE *f)
{
    int c;
    while ((c = fgetc(f)) != '\n' && c != EOF)
        ;
}

typedef enum
{
    QUIT = 0,
    INSERT,
    REMOVE,
    REMOVE_BELOW,
    FIND,
    PRINT,
    GRAPHVIZ,
} menu_item;

void print_menu()
{
    interface_printf("0 - выйти\n");
    interface_printf("1 - вставка\n");
    interface_printf("2 - удаление\n");
    interface_printf("3 - удаление всех со средним баллом < 2.0\n");
    interface_printf("4 - поиск по фамилии\n");
    interface_printf("5 - вывод 3 обходов\n");
    interface_printf("6 - вывод в graphviz\n");
}

int main(int argc, char **argv)
{
    if (argc == 2 && !strcmp(argv[1], "p")){
        error rc = run_perf_test();
        return rc;
    }
    menu_item action = -1;
    node_t *root = NULL;
    while (action)
    {
        interface_printf("Введите действие:\n");
        print_menu();
        if (scanf("%d", (int *)(&action)) != 1)
        {
            interface_printf("Ошибка %d\n", WRONG_INPUT);
            tree_delete(root);
            return WRONG_INPUT;
        }
        scan_newlines(stdin);
        switch (action)
        {
        case QUIT:
            tree_delete(root);
            return 0;
        case INSERT: {
            char *surname = NULL;
            double gpa = 0;
            size_t dummy = 0;
            interface_printf("Введите фамилию студента, затем его средний балл\n");
            if (getline(&surname, &dummy, stdin) == -1)
            {
                free(surname);
                tree_delete(root);
                interface_printf("Ошибка %d\n", WRONG_INPUT);
                return WRONG_INPUT;
            }
            if (!strchr(surname, '\n') || strcmp(surname, "\n") == 0)
            {
                free(surname);
                tree_delete(root);
                interface_printf("Ошибка %d\n", WRONG_INPUT);
                return WRONG_INPUT;
            }
            surname[strcspn(surname, "\n")] = '\0';
            if (scanf("%lf", &gpa) != 1)
            {
                free(surname);
                tree_delete(root);
                interface_printf("Ошибка %d\n", WRONG_GPA_INPUT);
                return WRONG_GPA_INPUT;
            }
            scan_newlines(stdin);
            error rc = tree_insert(&root, comp_node_surname, surname, gpa);
            if (rc)
            {
                if (rc == DUPLICATE_KEY)
                {
                    interface_printf("Ключ %s уже существует в дереве!\n", surname);
                    free(surname);
                    break;
                }
                else
                {
                    free(surname);
                    tree_delete(root);
                    interface_printf("Ошибка %d\n", rc);
                    return rc;
                }
            }
            free(surname);
            break;
        }
        case REMOVE: {
            char *surname = NULL;
            size_t dummy = 0;
            interface_printf("Введите фамилию студента\n");
            if (getline(&surname, &dummy, stdin) == -1)
            {
                free(surname);
                tree_delete(root);
                interface_printf("Ошибка %lf\n", WRONG_INPUT);
                return WRONG_INPUT;
            }
            if (!strchr(surname, '\n') || strcmp(surname, "\n") == 0)
            {
                free(surname);
                tree_delete(root);
                interface_printf("Ошибка %d\n", WRONG_INPUT);
                return WRONG_INPUT;
            }
            surname[strcspn(surname, "\n")] = '\0';
            error rc = tree_remove(&root, comp_node_surname, surname);
            if (rc)
            {
                if (rc == KEY_NOT_FOUND)
                {
                    interface_printf("Ключ %s в дереве не найден!\n", surname);
                    free(surname);
                    break;
                }
                else
                {
                    free(surname);
                    tree_delete(root);
                    interface_printf("Ошибка %d\n", rc);
                    return rc;
                }
            }
            break;
        }
        case REMOVE_BELOW: {
            FILE *fout_before = fopen("tree_before.dot", "w"), *fout_after = fopen("tree_after.dot", "w");
            if (!fout_before || !fout_after)
            {
                tree_delete(root);
                interface_printf("Ошибка %d\n", WRONG_OUT_FILE);
                return WRONG_OUT_FILE;
            }
            serialize_to_graphviz(root, fout_before);
            fclose(fout_before);
            error rc = tree_remove_below(&root, comp_node_surname, 2.0);
            if (rc)
            {
                tree_delete(root);
                fclose(fout_after);
                interface_printf("Ошибка %d\n", rc);
                return rc;
            }
            serialize_to_graphviz(root, fout_after);
            fclose(fout_after);
            interface_printf("Команда dot -Tpng tree_before.dot -o tree_before.png\n");
            interface_printf("Команда dot -Tpng tree_after.dot -o tree_after.png\n");
            break;
        }
        case FIND: {
            char *surname = NULL;
            size_t dummy = 0;
            interface_printf("Введите фамилию студента\n");
            if (getline(&surname, &dummy, stdin) == -1)
            {
                free(surname);
                tree_delete(root);
                interface_printf("Ошибка %lf\n", WRONG_INPUT);
                return WRONG_INPUT;
            }
            if (!strchr(surname, '\n') || strcmp(surname, "\n") == 0)
            {
                free(surname);
                tree_delete(root);
                interface_printf("Ошибка %d\n", WRONG_INPUT);
                return WRONG_INPUT;
            }
            surname[strcspn(surname, "\n")] = '\0';
            node_t *res = NULL;
            error rc = tree_find(root, comp_node_surname, surname, &res);
            if (rc)
            {
                if (rc == KEY_NOT_FOUND)
                {
                    interface_printf("Ключ %s в дереве не найден!\n", surname);
                    free(surname);
                    break;
                }
                else
                {
                    free(surname);
                    tree_delete(root);
                    interface_printf("Ошибка %d\n", rc);
                    return rc;
                }
            }
            interface_printf("Фамилия и средний балл:\n");
            node_print(res);
            break;
        }
        case PRINT: {
            interface_printf("Префиксный обход (pre-order):\n");
            tree_walk(root, PREFIX);
            interface_printf("Инфиксный обход (in-order):\n");
            tree_walk(root, INFIX);
            interface_printf("Постфиксный обход (post-order):\n");
            tree_walk(root, POSTFIX);
            break;
        }
        case GRAPHVIZ: {
            char *filename = NULL;
            size_t dummy = 0;
            interface_printf("Введите имя выходного файла\n");
            if (getline(&filename, &dummy, stdin) == -1)
            {
                interface_printf("Ошибка %d\n", WRONG_INPUT);
                free(filename);
                tree_delete(root);
                return WRONG_INPUT;
            }
            if (!strchr(filename, '\n') || strcmp(filename, "\n") == 0)
            {
                free(filename);
                tree_delete(root);
                interface_printf("Ошибка %d\n", WRONG_INPUT);
                return WRONG_INPUT;
            }
            filename[strcspn(filename, "\n")] = '\0';
            FILE *fout = fopen(filename, "w");
            if (!fout)
            {
                interface_printf("Ошибка %d\n", WRONG_OUT_FILE);
                free(filename);
                break;
            }
            serialize_to_graphviz(root, fout);
            interface_printf("Команда dot -Tpng %s -o %s\n", filename, "tree.png");
            fclose(fout);
            free(filename);
            break;
        }
        default:
            interface_printf("Ошибка %d\n", UNKNOWN_ACTION);
        }
    }
}