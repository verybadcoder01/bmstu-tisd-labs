#include "errors.h"
#include "graph.h"
#include "stdio.h"
#include <stdlib.h>

int main(void)
{
    int vert_count = 0;
    printf("Введите число вершин\n");
    if (scanf("%d", &vert_count) != 1)
    {
        return INPUT_ERROR;
    }
    if (vert_count <= 0)
    {
        return WRONG_DIM;
    }
    graph gr = { NULL, 0 };
    error rc = graph_alloc(&gr, vert_count);
    if (rc)
    {
        printf("Ошибка %d\n", rc);
        return rc;
    }
    int edge_count = 0;
    printf("Введите число ребер\n");
    if (scanf("%d", &edge_count) != 1)
    {
        free_graph(&gr);
        printf("Ошибка %d\n", INPUT_ERROR);
        return INPUT_ERROR;
    }
    if (edge_count < 0)
    {
        free_graph(&gr);
        printf("Ошибка %d\n", WRONG_DIM);
        return WRONG_DIM;
    }
    if (edge_count == 0) {
        free_graph(&gr);
        printf("Пустой граф!\n");
        return WRONG_DIM;
    }
    rc = scan_graph(&gr, edge_count);
    if (rc)
    {
        printf("Ошибка %d\n", rc);
        free_graph(&gr);
        return rc;
    }
    rc = serialize_graph_simple(&gr, "input.dot");
    if (rc)
    {
        printf("Ошибка %d\n", rc);
        free_graph(&gr);
        return rc;
    }
    printf("Исходный граф в файле input.dot\n");
    vert_list result = { NULL, NULL, 0, 0 };
    rc = find_euler_path(&gr, &result);
    if (rc)
    {
        if (rc == EULER_NO_PATH || rc == EULER_DISCONNECTED)
        {
            printf("В графе не существует Эйлерова пути!\n");
        }
        else
        {
            printf("Ошибка %d\n", rc);
        }
        free_graph(&gr);
        free_vert_list(&result);
        return rc;
    }
    rc = serialize_euler_path_to_dot(&gr, &result, "result.dot");
    free_graph(&gr);
    free_vert_list(&result);
    if (rc)
    {
        printf("Ошибка %d\n", rc);
        return rc;
    }
    printf("Результат в файле result.dot\n");
    return 0;
}