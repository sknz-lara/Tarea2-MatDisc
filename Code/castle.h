#ifndef CASTLE_H
#define CASTLE_H

// ==== ESTRUCTURAS ====

typedef struct {
    int *neighbors;
    int count;
    int cap;
} AdjList;

typedef struct {
    int P;      // pisos
    int X;      // ancho
    int Y;      // alto
    int n_nodes;
    AdjList *adj;
} Graph;

// ==== FUNCIONES ====

Graph  graph_create(int P, int X, int Y);
void   graph_destroy(Graph *g);
long long graph_edge_count_directed(const Graph *g);

// (Opcional: funciones adicionales)
int    id_from_pxy(const Graph *g, int p, int x, int y);

#endif
