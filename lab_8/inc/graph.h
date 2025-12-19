#ifndef GRAPH_H__
#define GRAPH_H__

#include "errors.h"
#include "stdbool.h"

typedef struct {
    int *vertices;
    bool *active;
    int size, cap;
} vert_list;

typedef struct {
    vert_list *inc_list;
    int size;
} graph;

error add_vert(vert_list *vl, int num);

error graph_alloc(graph *gr, int vert_count);

error scan_graph(graph *gr, int edge_count);

error find_euler_path(graph *gr, vert_list *result);

error serialize_graph_simple(graph *gr, const char *filename);

error serialize_euler_path_to_dot(graph *gr, vert_list *path, const char *filename);

void free_vert_list(vert_list *vl);

void free_graph(graph *gr);

#endif