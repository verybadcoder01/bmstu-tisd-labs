#include "stdio.h"
#include "table.h"
#include "io.h"
#include "malloc.h"

typedef enum
{
    READ_FILE = 1,
    PUSH_BACK = 2,
    DELETE_BY_TITLE = 3,
    SORT_KEYS = 4,
    SORT_ORIGIN = 5,
    SORT_VIA_KEY_TABLE = 6,
    COMPARE_ORIGIN_KEYS = 7,
    PRINT_BY_VARIANT = 8,
    PRINT_TABLE = 9,
    PRINT_KEY_TABLE = 10,
    QUIT = 0
} menu_item;

#define PERF_TEST_RUNS 10
const char perf_test_files[5][20] = {"1000_rand.txt", "2000_rand.txt", "5000_rand.txt", "5000_sorted.txt", "5000_rev.txt"};

int main(void)
{
    table *tab = NULL;
    while (1)
    {
        interface_printf("Выберите действие\n");
        interface_printf("1 - считать данные таблицы из файла\n");
        interface_printf("2 - добавить элемент в конец таблицы\n");
        interface_printf("3 - удалить книгу по названию\n");
        interface_printf("4 - просмотр отсортированной таблицы ключей при несортированной исходной таблице\n");
        interface_printf("5 - сортировка исходной таблицы\n");
        interface_printf("6 - вывод исходной таблицы в упорядоченном виде, используя упорядоченную таблицу ключе\n");
        interface_printf("7 - вывод результатов сравнения эффективности работы программы при обработке данных в исходной таблице и в таблице ключей.\nИспользуются заранее заготовленные тестовые данные\n");
        interface_printf("8 - вывести список всех романов указанного автора\n");
        interface_printf("9 - вывести таблицу\n");
        interface_printf("10 - вывести таблицу ключей\n");
        interface_printf("0 - завершить программу\n");
        menu_item action;
        scanf("%d", (int *)&action);
        switch (action)
        {
        case QUIT:
            delete_table(tab);
            return 0;
        case READ_FILE:
        {
            char *file_name = NULL;
            size_t len = 0;
            interface_printf("Введите имя файла\n");
            error rc = scan(stdin, &file_name, &len);
            if (rc)
            {
                interface_print_error(rc);
                return rc;
            }
            FILE *f = fopen(file_name, "r");
            if (!f)
            {
                free(file_name);
                interface_print_error(FILE_NOT_ACCESSIBLE);
                return FILE_NOT_ACCESSIBLE;
            }
            tab = malloc(sizeof(table));
            if (!tab)
            {
                free(file_name);
                fclose(f);
                return ALLOC_ERROR;
            }
            rc = init_table_from_file(f, tab);
            free(file_name);
            fclose(f);
            if (rc)
            {
                delete_table(tab);
                interface_print_error(rc);
                return rc;
            }
            print_table(tab);
        }
        break;
        case PUSH_BACK:
        {
            if (!tab)
            {
                tab = malloc(sizeof(table));
                if (!tab)
                {
                    return ALLOC_ERROR;
                }
                init_default_table(tab);
            }
            book *n_book = malloc(sizeof(book));
            if (!n_book)
            {
                return ALLOC_ERROR;
            }
            error rc = scan_book(stdin, n_book);
            if (rc && rc != EOF)
            {
                delete_table(tab);
                delete_book(n_book);
                interface_print_error(rc);
                return rc;
            }
            rc = push_back(tab, n_book);
            delete_book(n_book);
            if (rc)
            {
                delete_table(tab);
                interface_print_error(rc);
                return rc;
            }
        }
        break;
        case DELETE_BY_TITLE:
        {
            if (!tab)
            {
                interface_print_error(NO_TABLE);
                return NO_TABLE;
            }
            char *target_title = NULL;
            size_t size = 0;
            interface_printf("Введите название книги\n");
            error rc = scan(stdin, &target_title, &size);
            if (rc)
            {
                delete_table(tab);
                interface_print_error(rc);
                return rc;
            }
            rc = remove_elem(tab, target_title);
            free(target_title);
            if (rc)
            {
                delete_table(tab);
                interface_print_error(rc);
                return rc;
            }
        }
        break;
        case SORT_KEYS:
        {
            if (!tab)
            {
                interface_print_error(NO_TABLE);
                return NO_TABLE;
            }
            sort_type method = 0;
            interface_printf("Введите метод сортировки: 1 - пузырьком, 2 - слиянием\n");
            if (scanf("%d", (int *)&method) != 1 || !valid_sort_type(method))
            {
                delete_table(tab);
                interface_print_error(INPUT_ERROR);
                return INPUT_ERROR;
            }
            error rc = sort_key_table(tab, method);
            if (rc)
            {
                delete_table(tab);
                interface_print_error(rc);
                return rc;
            }
            print_key_table(tab);
        }
        break;
        case SORT_ORIGIN:
        {
            if (!tab)
            {
                return NO_TABLE;
            }
            sort_type method = 0;
            interface_printf("Введите метод сортировки: 1 - пузырьком, 2 - слиянием\n");
            if (scanf("%d", (int *)&method) != 1 || !valid_sort_type(method))
            {
                delete_table(tab);
                interface_print_error(INPUT_ERROR);
                return INPUT_ERROR;
            }
            if (method == BUBBLE)
            {
                bubble_sort(tab->data, tab->size, sizeof(book), &comp_book_t);
            }
            else
            {
                error rc = merge_sort(tab->data, tab->size, sizeof(book), &comp_book_t);
                if (rc)
                {
                    delete_table(tab);
                    interface_print_error(rc);
                    return rc;
                }
            }
            print_table(tab);
        }
        break;
        case SORT_VIA_KEY_TABLE:
        {
            if (!tab)
            {
                interface_print_error(NO_TABLE);
                return NO_TABLE;
            }
            sort_type method = 0;
            interface_printf("Введите метод сортировки: 1 - пузырьком, 2 - слиянием\n");
            if (scanf("%d", (int *)&method) != 1 || !valid_sort_type(method))
            {
                delete_table(tab);
                interface_print_error(INPUT_ERROR);
                return INPUT_ERROR;
            }
            error rc = print_sorted_via_key_table(tab, method);
            if (rc)
            {
                delete_table(tab);
                interface_print_error(rc);
                return rc;
            }
        }
        break;
        case COMPARE_ORIGIN_KEYS:
        {
            for (size_t i = 0; i < 5; ++i)
            {
                double avg_merge_orig = 0, avg_merge_keys = 0, avg_bubble_orig = 0, avg_bubble_keys = 0;
                FILE *f = fopen(perf_test_files[i], "r");
                if (!f)
                {
                    interface_print_error(FILE_NOT_ACCESSIBLE);
                    return FILE_NOT_ACCESSIBLE;
                }
                for (int run = 0; run < PERF_TEST_RUNS; ++run)
                {
                    unsigned long merge_orig = 0, merge_keys = 0, bubble_orig = 0, bubble_keys = 0;
                    error rc = self_perf_test(f, &bubble_orig, &merge_orig, &bubble_keys, &merge_keys);
                    if (rc)
                    {
                        fclose(f);
                        interface_print_error(rc);
                        return rc;
                    }
                    avg_bubble_orig += bubble_orig;
                    avg_bubble_keys += bubble_keys;
                    avg_merge_orig += merge_orig;
                    avg_merge_keys += merge_keys;
                }
                fclose(f);
                avg_bubble_keys /= (double)PERF_TEST_RUNS;
                avg_bubble_orig /= (double)PERF_TEST_RUNS;
                avg_merge_keys /= (double)PERF_TEST_RUNS;
                avg_merge_orig /= (double)PERF_TEST_RUNS;
                printf("Результат сортировки оригинальной таблицы на файле %s:\n", perf_test_files[i]);
                printf("С помощью сортировки пузырьком %lf, слиянием %lf\n", avg_bubble_orig, avg_merge_orig);
                printf("Результат сортировки таблицы ключей на файле %s:\n", perf_test_files[i]);
                printf("С помощью сортировки пузырьком %lf, слиянием %lf\n", avg_bubble_keys, avg_merge_keys);
            }
        }
        break;
        case PRINT_BY_VARIANT:
        {
            if (!tab)
            {
                interface_print_error(NO_TABLE);
                return NO_TABLE;
            }
            char *target_title = NULL;
            size_t size = 0;
            interface_printf("Введите интересующего автора\n");
            error rc = scan(stdin, &target_title, &size);
            if (rc)
            {
                delete_table(tab);
                interface_print_error(rc);
                return rc;
            }
            print_by_variant(tab, target_title);
            free(target_title);
        }
        break;
        case PRINT_TABLE:
            if (!tab)
            {
                interface_print_error(NO_TABLE);
                return NO_TABLE;
            }
            print_table(tab);
            break;
        case PRINT_KEY_TABLE:
            if (!tab)
            {
                interface_print_error(NO_TABLE);
                return NO_TABLE;
            }
            print_key_table(tab);
            break;
        default:
            interface_printf("Неверный выбор.\n");
            delete_table(tab);
            return WRONG_MENU_ITEM;
        }
    }
    return 0;
}