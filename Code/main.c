#include <stdio.h>
#include <stdlib.h>
#include "castle.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s P X Y\n", argv[0]);
        return EXIT_FAILURE;
    }

    int P = atoi(argv[1]);
    int X = atoi(argv[2]);
    int Y = atoi(argv[3]);

    Graph g = graph_create(P, X, Y);

    printf("Castillo creado con %d nodos.\n", g.n_nodes);
    printf("Aristas (aprox): %lld\n", graph_edge_count_directed(&g) / 2);

    graph_destroy(&g);
    return 0;
}
