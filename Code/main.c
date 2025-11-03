#include <stdio.h>
#include <stdlib.h>
#include "castle.h"
#include "draft.h"

static int clampi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s P X Y\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 2 4 3\n", argv[0]);
        return EXIT_FAILURE;
    }

    int P = atoi(argv[1]);
    int X = atoi(argv[2]);
    int Y = atoi(argv[3]);

    if (P <= 0 || X <= 0 || Y <= 0) {
        fprintf(stderr, "Error: P, X, Y deben ser positivos.\n");
        return EXIT_FAILURE;
    }

    Graph g = create(P, X, Y);

    printf("Castillo creado con %d nodos (%d pisos de %dx%d)\n", g.n_nodes, P, X, Y);
    printf("Aristas iniciales (aprox): %lld\n", edge_count(&g) / 2);

    int ex1 = (X >= 2) ? 1 : 0;
    int ey1 = (Y >= 2) ? 1 : 0;
    add_enemy(&g, 0, ex1, ey1, 5);
    if (X >= 4 && Y >= 3) add_enemy(&g, 0, X - 1, Y - 2, 9);
    if (P >= 2 && X >= 3 && Y >= 2) add_enemy(&g, 1, X - 2, 1, 7);

    if (X >= 3) {
        int kx = clampi(X / 2, 1, X - 1);
        add_wall(&g, 0, kx, 0, kx, Y - 1);
    }
    if (Y >= 2) {
        int ky = clampi(Y / 2, 1, Y - 1);
        if (P >= 2) add_wall(&g, 1, 0, ky, X - 1, ky);
        else add_wall(&g, 0, 0, ky, X - 1, ky);
    }

    if (P >= 2) {
        int sx = X / 2;
        int sy = Y / 2;
        add_stairs(&g, 0, sx, sy);
        if (P >= 3) {
            int sx2 = (X > 1) ? X - 1 : 0;
            int sy2 = (Y > 1) ? Y - 1 : 0;
            add_stairs(&g, 1, sx2, sy2);
        }
    }

    if (P >= 2) {
        add_portal(&g, 0, 0, 0, 1, X - 1, Y - 1);
    } else {
        if (X > 1 || Y > 1) add_portal(&g, 0, 0, 0, 0, X - 1, Y - 1);
    }

    printf("Aristas tras modificaciones (aprox): %lld\n", edge_count(&g) / 2);

    if (draft_render_txt(&g, "castle.txt") == 0)
        printf("Archivo 'castle.txt' generado correctamente.\n");
    else
        fprintf(stderr, "Error: no se pudo generar 'castle.txt'\n");

    destroy(&g);
    return EXIT_SUCCESS;
}
