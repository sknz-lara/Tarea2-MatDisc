#include <stdio.h>
#include <stdlib.h>
#include "castle.h"

static void neigh_push(Node *n, Node *v) {
    Node **tmp = (Node**)realloc(n->neighbors, (n->count + 1) * sizeof(Node*));
    if (!tmp) {
        fprintf(stderr, "Error: sin memoria en neigh_push\n");
        exit(EXIT_FAILURE);
    }
    n->neighbors = tmp;
    n->neighbors[n->count++] = v;
}

static int id_from_pxy(const Graph *g, int p, int x, int y) {
    return p * (g->Y * g->X) + y * g->X + x;
}

static Node* node_from_pxy(Graph *g, int p, int x, int y) {
    return &g->nodes[id_from_pxy(g, p, x, y)];
}

static void remove_neighbor_one_way(Node *a, Node *b) {
    if (!a || !b || !a->neighbors) return;
    for (int i = 0; i < a->count; ++i) {
        if (a->neighbors[i] == b) {
            for (int j = i + 1; j < a->count; ++j)
                a->neighbors[j - 1] = a->neighbors[j];
            a->count--;
            if (a->count == 0) {
                free(a->neighbors);
                a->neighbors = NULL;
            } else {
                Node **tmp = (Node**)realloc(a->neighbors, a->count * sizeof(Node*));
                if (!tmp && a->count > 0) {
                    return;
                }
                a->neighbors = tmp;
            }
            return;
        }
    }
}

static void remove_edge_bidirectional(Node *a, Node *b) {
    if (!a || !b) return;
    remove_neighbor_one_way(a, b);
    remove_neighbor_one_way(b, a);
}

static int min_int(int a, int b) { return (a < b) ? a : b; }
static int max_int(int a, int b) { return (a > b) ? a : b; }


Graph create(int P, int X, int Y) {
    Graph g;
    g.P = P; g.X = X; g.Y = Y;
    g.n_nodes = P * X * Y;

    g.nodes = (Node*)calloc(g.n_nodes, sizeof(Node));
    if (!g.nodes) {
        fprintf(stderr, "Error: sin memoria para los nodos (%d nodos)\n", g.n_nodes);
        exit(EXIT_FAILURE);
    }

    for (int p = 0; p < P; ++p) {
        for (int y = 0; y < Y; ++y) {
            for (int x = 0; x < X; ++x) {
                int id = p * (Y * X) + y * X + x;
                Node *n = &g.nodes[id];
                n->id = id;
                n->p = p; n->x = x; n->y = y;
                n->neighbors = NULL;
                n->count = 0;
                n->enemy_cost = 0;
            }
        }
    }
    for (int p = 0; p < P; ++p) {
        for (int y = 0; y < Y; ++y) {
            for (int x = 0; x < X; ++x) {
                Node *u = node_from_pxy(&g, p, x, y);
                if (x > 0)     neigh_push(u, node_from_pxy(&g, p, x - 1, y));
                if (x + 1 < X) neigh_push(u, node_from_pxy(&g, p, x + 1, y));
                if (y > 0)     neigh_push(u, node_from_pxy(&g, p, x, y - 1));
                if (y + 1 < Y) neigh_push(u, node_from_pxy(&g, p, x, y + 1));
            }
        }
    }

    return g;
}

void destroy(Graph *g) {
    if (!g || !g->nodes) return;
    for (int i = 0; i < g->n_nodes; ++i) {
        free(g->nodes[i].neighbors);
        g->nodes[i].neighbors = NULL;
        g->nodes[i].count = 0;
    }
    free(g->nodes);
    g->nodes = NULL;
    g->n_nodes = 0;
}

long long edge_count(const Graph *g) {
    long long m = 0;
    for (int i = 0; i < g->n_nodes; ++i) m += g->nodes[i].count;
    return m;
}

Node* get_node(Graph *g, int p, int x, int y) {
    if (!g || !g->nodes) return NULL;
    if (p < 0 || p >= g->P || x < 0 || x >= g->X || y < 0 || y >= g->Y) return NULL;
    return &g->nodes[id_from_pxy(g, p, x, y)];
}


int add_enemy(Graph *g, int p, int x, int y, int cost) {
    if (!g || !g->nodes || cost < 0) return -1;
    Node *n = get_node(g, p, x, y);
    if (!n) return -1;
    n->enemy_cost = cost;
    return 0;
}

int add_wall(Graph *g, int p, int x1, int y1, int x2, int y2) {
    if (!g || !g->nodes) return -1;
    if (p < 0 || p >= g->P) return -1;
    if (!(x1 == x2 || y1 == y2)) return -1;

    int xa = min_int(x1, x2), xb = max_int(x1, x2);
    int ya = min_int(y1, y2), yb = max_int(y1, y2);

    if (x1 == x2) {
        int k = x1;
        if (k <= 0 || k >= g->X) return -1;
        int ys = (ya < 0) ? 0 : ya;
        int ye = (yb >= g->Y) ? (g->Y - 1) : yb;
        for (int y = ys; y <= ye; ++y) {
            Node *a = get_node(g, p, k - 1, y);
            Node *b = get_node(g, p, k,     y);
            if (a && b) remove_edge_bidirectional(a, b);
        }
        return 0;
    }

    if (y1 == y2) {
        int k = y1;
        if (k <= 0 || k >= g->Y) return -1;
        int xs = (xa < 0) ? 0 : xa;
        int xe = (xb >= g->X) ? (g->X - 1) : xb;
        for (int x = xs; x <= xe; ++x) {
            Node *a = get_node(g, p, x, k - 1);
            Node *b = get_node(g, p, x, k);
            if (a && b) remove_edge_bidirectional(a, b);
        }
        return 0;
    }

    return -1;
}

int add_portal(Graph *g, int p1, int x1, int y1, int p2, int x2, int y2) {
    Node *a = get_node(g, p1, x1, y1);
    Node *b = get_node(g, p2, x2, y2);
    if (!a || !b) return -1;
    neigh_push(a, b);
    neigh_push(b, a);
    return 0;
}

int add_stairs(Graph *g, int p, int x, int y) {
    if (p < 0 || p >= g->P - 1) return -1;
    Node *a = get_node(g, p,     x, y);
    Node *b = get_node(g, p + 1, x, y);
    if (!a || !b) return -1;
    neigh_push(a, b);
    neigh_push(b, a);
    return 0;
}
