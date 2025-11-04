#ifndef CASTLE_H
#define CASTLE_H

typedef struct Node {
    int id;
    int p, x, y;
    struct Node **neighbors;
    int count;
    int enemy_cost;
} Node;

typedef struct {
    int P;
    int X;
    int Y;
    int n_nodes;
    Node *nodes;
} Graph;

Graph  create(int P, int X, int Y);
void   destroy(Graph *g);
long long edge_count(const Graph *g);
Node*  get_node(Graph *g, int p, int x, int y);

int add_enemy (Graph *g, int p, int x, int y, int cost);
int add_wall  (Graph *g, int p, int x1, int y1, int x2, int y2);
int add_portal(Graph *g, int p1, int x1, int y1, int p2, int x2, int y2);
int add_stairs(Graph *g, int p, int x, int y);

#endif
