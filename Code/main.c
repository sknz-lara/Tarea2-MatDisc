#include <stdio.h>
#include <stdlib.h>
#include "castle.h"
#include "draft.h"
#include "reader.h"
#include "bfs.h"

static int clampi(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

int find_node_id(const Graph *g, int p, int x, int y);

int main(void) {
    char archivo[128];
    printf("Ingrese el nombre del archivo del castillo: ");
    scanf("%s", archivo);

    Castillo c;
    lectura_archivo(archivo, &c);

    printf("Castillo leído: %d pisos, ancho %d y alto %d\n", c.pisos, c.ancho, c.alto);
    printf("Hrongan en: piso %d, (%d,%d)\n", c.hrongan.piso, c.hrongan.x, c.hrongan.y);
    printf("Salida en: piso %d, (%d,%d)\n", c.salida.piso, c.salida.x, c.salida.y);

    Graph g = create(c.pisos, c.ancho, c.alto);

    for (int i = 0; i < c.num_muros; ++i)
        add_wall(&g, c.muros[i].inicio.piso, c.muros[i].inicio.x, c.muros[i].inicio.y,
                      c.muros[i].x_fin, c.muros[i].y_fin);

    for (int i = 0; i < c.num_monstruos; ++i)
        add_enemy(&g, c.monstruos[i].posicion_monstruo.piso,
                      c.monstruos[i].posicion_monstruo.x,
                      c.monstruos[i].posicion_monstruo.y,
                      c.monstruos[i].vidas);

    for (int i = 0; i < c.num_portales; ++i)
        add_portal(&g, c.portales[i].portal_inicio.piso,
                      c.portales[i].portal_inicio.x, c.portales[i].portal_inicio.y,
                      c.portales[i].portal_fin.piso,
                      c.portales[i].portal_fin.x, c.portales[i].portal_fin.y);

    for (int i = 0; i < c.num_escaleras; ++i)
        add_stairs(&g, c.escaleras[i].posicion_inferior.piso,
                      c.escaleras[i].posicion_inferior.x,
                      c.escaleras[i].posicion_inferior.y);

    int inicio_id = find_node_id(&g, c.hrongan.piso, c.hrongan.x, c.hrongan.y);
    int salida_id = find_node_id(&g, c.salida.piso, c.salida.x, c.salida.y);
    printf("Nodo inicio_id = %d, salida_id = %d\n", inicio_id, salida_id);


    if (inicio_id == -1 || salida_id == -1) {
        fprintf(stderr, "Error: no se pudo encontrar el nodo inicial o final.\n");
        destroy(&g);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < g.n_nodes; ++i) {
    Node *n = &g.nodes[i];
    if (n->count == 0) continue;
    printf("Nodo [%d,%d,%d] -> ", n->p, n->x, n->y);
    for (int j = 0; j < n->count; ++j) {
        Node *v = n->neighbors[j];
        printf("[%d,%d,%d] ", v->p, v->x, v->y);
    }
    printf("\n");
    }

    int vidas;
    printf("Ingrese vidas iniciales: ");
    scanf("%d", &vidas);

    printf("El grafo tiene %lld aristas en total.\n", edge_count(&g));
    int total_vecinos = 0;
    for (int i = 0; i < g.n_nodes; ++i) total_vecinos += g.nodes[i].count;
    printf("Promedio de vecinos por nodo: %.2f\n", (float)total_vecinos / g.n_nodes);


    bfs_camino(&g, inicio_id, salida_id, vidas);

    destroy(&g);
    return 0;
}
