#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *neighbors;
    int count;
    int cap;
} AdjList;

typedef struct {
    int P;
    int X;
    int Y;
    int n_nodes;
    AdjList *adj;
} Graph;

static void adj_push(AdjList *al, int v) {
    if (al->count == al->cap) {
        int new_cap = (al->cap == 0) ? 4 : al->cap * 2;
        int *tmp = (int*)realloc(al->neighbors, new_cap * sizeof(int));
        if (!tmp) {
            fprintf(stderr, "Error: sin memoria en adj_push\n");
            exit(EXIT_FAILURE);
        }
        al->neighbors = tmp;
        al->cap = new_cap;
    }
    al->neighbors[al->count++] = v;
}

int id_from_pxy(const Graph *g, int p, int x, int y) {
    return p * (g->Y * g->X) + y * g->X + x;
}

Graph graph_create(int P, int X, int Y) {
    Graph g;
    g.P = P; g.X = X; g.Y = Y;
    g.n_nodes = P * X * Y;

    g.adj = (AdjList*)calloc(g.n_nodes, sizeof(AdjList));
    if (!g.adj) {
        fprintf(stderr, "Error: sin memoria para el grafo (%d nodos)\n", g.n_nodes);
        exit(EXIT_FAILURE);
    }
    for (int p = 0; p < P; ++p) {
        for (int y = 0; y < Y; ++y) {
            for (int x = 0; x < X; ++x) {
                int u = id_from_pxy(&g, p, x, y);

                if (x - 1 >= 0) adj_push(&g.adj[u], id_from_pxy(&g, p, x - 1, y)); // izquierda
                if (x + 1 < X)  adj_push(&g.adj[u], id_from_pxy(&g, p, x + 1, y)); // derecha
                if (y - 1 >= 0) adj_push(&g.adj[u], id_from_pxy(&g, p, x, y - 1)); // arriba
                if (y + 1 < Y)  adj_push(&g.adj[u], id_from_pxy(&g, p, x, y + 1)); // abajo
            }
        }
    }

    return g;
}

void graph_destroy(Graph *g) {
    if (!g || !g->adj) return;
    for (int i = 0; i < g->n_nodes; ++i) free(g->adj[i].neighbors);
    free(g->adj);
    g->adj = NULL;
    g->n_nodes = 0;
}

long long graph_edge_count_directed(const Graph *g) {
    long long m = 0;
    for (int i = 0; i < g->n_nodes; ++i) m += g->adj[i].count;
    return m;
}
