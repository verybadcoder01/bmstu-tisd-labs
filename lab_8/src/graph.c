#include "graph.h"
#include "errors.h"
#include "stdio.h"
#include <stdlib.h>

error add_vert(vert_list *vl, int num)
{
    if (vl->size >= vl->cap)
    {
        int n_cap = vl->cap * 2;
        if (vl->cap == 0)
        {
            n_cap = 1;
        }
        int *tmp_buf = realloc(vl->vertices, n_cap * sizeof(int));
        if (!tmp_buf)
        {
            return ALLOC_ERROR;
        }
        bool *tmp_buf2 = realloc(vl->active, n_cap * sizeof(int));
        if (!tmp_buf2)
        {
            return ALLOC_ERROR;
        }
        vl->vertices = tmp_buf;
        vl->active = tmp_buf2;
        vl->cap = n_cap;
    }
    vl->vertices[vl->size] = num;
    vl->active[vl->size] = 1;
    vl->size++;
    return 0;
}

void free_vert_list(vert_list *vl)
{
    free(vl->vertices);
    free(vl->active);
}

error graph_alloc(graph *gr, int vert_count)
{
    gr->inc_list = malloc(vert_count * sizeof(vert_list));
    if (!gr->inc_list)
    {
        return ALLOC_ERROR;
    }
    for (int i = 0; i < vert_count; ++i)
    {
        gr->inc_list[i].vertices = NULL;
        gr->inc_list[i].active = NULL;
        gr->inc_list[i].size = 0;
        gr->inc_list[i].cap = 0;
    }
    gr->size = vert_count;
    return 0;
}

error scan_graph(graph *gr, int edge_count)
{
    printf("Введите ребра в формате u v, в 0-индексации\n");
    for (int i = 0; i < edge_count; ++i)
    {
        int u = 0, v = 0;
        if (scanf("%d %d", &u, &v) != 2)
        {
            return INPUT_ERROR;
        }
        if (u < 0 || v < 0 || u >= gr->size || v >= gr->size)
        {
            return WRONG_VERT_NUMBER;
        }
        error rc = add_vert(&gr->inc_list[u], v);
        if (rc)
        {
            return rc;
        }
        rc = add_vert(&gr->inc_list[v], u);
        if (rc)
        {
            return rc;
        }
    }
    return 0;
}

void free_graph(graph *gr)
{
    for (int i = 0; i < gr->size; ++i)
    {
        free(gr->inc_list[i].active);
        free(gr->inc_list[i].vertices);
    }
    free(gr->inc_list);
}

static int count_odd_vertices(graph *gr)
{
    int odd_count = 0;
    for (int i = 0; i < gr->size; i++)
    {
        int active_count = 0;
        for (int j = 0; j < gr->inc_list[i].size; j++)
        {
            if (gr->inc_list[i].active[j])
            {
                active_count++;
            }
        }
        if (active_count % 2 == 1)
        {
            odd_count++;
        }
    }
    return odd_count;
}

static void dfs(int v, graph *gr, int *visited)
{
    visited[v] = 1;

    for (int i = 0; i < gr->inc_list[v].size; i++)
    {
        if (gr->inc_list[v].active[i])
        {
            int neighbor = gr->inc_list[v].vertices[i];
            if (!visited[neighbor])
            {
                dfs(neighbor, gr, visited);
            }
        }
    }
}

static int is_connected(graph *gr)
{
    if (gr->size == 0)
        return 1;

    int *visited = calloc(gr->size, sizeof(int));
    if (!visited)
    {
        return ALLOC_ERROR;
    }

    int start = -1;
    for (int i = 0; i < gr->size; i++)
    {
        for (int j = 0; j < gr->inc_list[i].size; j++)
        {
            if (gr->inc_list[i].active[j])
            {
                start = i;
                break;
            }
        }
        if (start != -1)
        {
            break;
        }
    }

    if (start == -1)
    {
        free(visited);
        return 1;
    }

    dfs(start, gr, visited);
    int connected = 1;
    for (int i = 0; i < gr->size; i++)
    {
        for (int j = 0; j < gr->inc_list[i].size; j++)
        {
            if (gr->inc_list[i].active[j] && !visited[i])
            {
                connected = 0;
                break;
            }
        }
        if (!connected)
        {
            break;
        }
    }

    free(visited);
    return connected;
}

static void disable_edge(graph *gr, int u, int v)
{
    for (int i = 0; i < gr->inc_list[u].size; i++)
    {
        if (gr->inc_list[u].vertices[i] == v && gr->inc_list[u].active[i])
        {
            gr->inc_list[u].active[i] = false;
            break;
        }
    }

    if (u != v)
    {
        for (int i = 0; i < gr->inc_list[v].size; i++)
        {
            if (gr->inc_list[v].vertices[i] == u && gr->inc_list[v].active[i])
            {
                gr->inc_list[v].active[i] = false;
                break;
            }
        }
    }
}

static error euler_dfs(graph *gr, int v, vert_list *result)
{
    for (int i = 0; i < gr->inc_list[v].size; i++)
    {
        if (gr->inc_list[v].active[i])
        {
            int neighbor = gr->inc_list[v].vertices[i];
            disable_edge(gr, v, neighbor);
            error err = euler_dfs(gr, neighbor, result);
            if (err)
            {
                return err;
            }
        }
    }

    error err = add_vert(result, v);
    return err;
}

static void reverse_vert_list(vert_list *list)
{
    int i = 0, j = list->size - 1;
    while (i < j)
    {
        int temp = list->vertices[i];
        list->vertices[i] = list->vertices[j];
        list->vertices[j] = temp;
        bool temp2 = list->active[i];
        list->active[i] = list->active[j];
        list->active[j] = temp2;
        i++;
        j--;
    }
}

error find_euler_path(graph *gr, vert_list *result)
{
    if (gr == NULL || result == NULL || gr->size == 0)
    {
        return EULER_NO_PATH;
    }

    int odd_count = count_odd_vertices(gr);
    if (!is_connected(gr))
    {
        return EULER_DISCONNECTED;
    }

    if (odd_count != 0 && odd_count != 2)
    {
        return EULER_NO_PATH;
    }

    int start_vertex = 0;
    if (odd_count == 2)
    {
        for (int i = 0; i < gr->size; i++)
        {
            int active_count = 0;
            for (int j = 0; j < gr->inc_list[i].size; j++)
            {
                if (gr->inc_list[i].active[j])
                {
                    active_count++;
                }
            }
            if (active_count % 2 == 1)
            {
                start_vertex = i;
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < gr->size; i++)
        {
            for (int j = 0; j < gr->inc_list[i].size; j++)
            {
                if (gr->inc_list[i].active[j])
                {
                    start_vertex = i;
                    break;
                }
            }
            if (start_vertex != 0)
                break;
        }
    }

    result->size = 0;
    result->cap = 0;
    result->vertices = NULL;
    result->active = NULL;

    error err = euler_dfs(gr, start_vertex, result);
    if (err)
    {
        return err;
    }
    reverse_vert_list(result);
    return 0;
}

error serialize_euler_path_to_dot(graph *gr, vert_list *path, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        return ALLOC_ERROR;
    }

    fprintf(file, "graph G {\n");
    fprintf(file, "    // Euler Path Visualization\n");
    fprintf(file, "    graph [splines=true];\n");
    fprintf(file, "    node [shape=circle, style=filled];\n\n");
    fprintf(file, "    // Vertices\n");
    for (int i = 0; i < gr->size; i++)
    {
        bool in_path = false;
        for (int j = 0; j < path->size; j++)
        {
            if (path->vertices[j] == i)
            {
                in_path = true;
                break;
            }
        }
        if (path->size > 0)
        {
            if (i == path->vertices[0])
            {
                fprintf(file, "    %d [fillcolor=green]; // Start vertex\n", i);
            }
            else if (i == path->vertices[path->size - 2])
            {
                fprintf(file, "    %d [fillcolor=green]; // End vertex\n", i);
            }
            else if (in_path)
            {
                fprintf(file, "    %d [fillcolor=yellow];\n", i);
            }
            else
            {
                fprintf(file, "    %d [fillcolor=lightgray];\n", i);
            }
        }
        else
        {
            fprintf(file, "    %d;\n", i);
        }
    }
    fprintf(file, "\n");
    fprintf(file, "    // Edges - Euler path in red, other edges in gray\n");
    for (int u = 0; u < gr->size; u++)
    {
        vert_list *list_u = &gr->inc_list[u];
        for (int i = 0; i < list_u->size; i++)
        {
            int v = list_u->vertices[i];
            if (u <= v)
            {
                fprintf(file, "    %d -- %d;\n", u, v);
            }
        }
    }
    fprintf(file, "}\n");
    fclose(file);
    return 0;
}

error serialize_graph_simple(graph *gr, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        return ALLOC_ERROR;
    }
    fprintf(file, "graph G {\n");
    fprintf(file, "    node [shape=circle];\n\n");
    for (int u = 0; u < gr->size; u++)
    {
        vert_list *list_u = &gr->inc_list[u];
        for (int i = 0; i < list_u->size; i++)
        {
            int v = list_u->vertices[i];
            if (u <= v)
            {
                fprintf(file, "    %d -- %d", u, v);
                if (u == v)
                {
                    fprintf(file, " [style=bold]");
                }
                fprintf(file, ";\n");
            }
        }
    }
    fprintf(file, "}\n");
    fclose(file);
    return 0;
}