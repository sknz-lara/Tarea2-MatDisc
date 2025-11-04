#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // para strcspn
#include "castle.h"
#include "draft.h"
#include "reader.h"
#include "bfs.h"

int find_node_id(const Graph *g, int p, int x, int y);

/* Limpia hasta fin de línea para recuperar ante entradas inválidas */
static void clear_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

int main(void) {
    char respuesta;

    do {
        char archivo[128];

        printf("Ingrese el nombre del archivo del castillo: ");
        if (!fgets(archivo, sizeof archivo, stdin)) {
            fprintf(stderr, "Error leyendo el nombre del archivo.\n");
            return 1;
        }
        /* quitar el '\n' final si viene */
        archivo[strcspn(archivo, "\r\n")] = '\0';
        if (archivo[0] == '\0') {
            fprintf(stderr, "Nombre de archivo vacío.\n");
            printf("¿Desea intentar con otro castillo? (y/n): ");
            if (scanf(" %c", &respuesta) != 1) {
                fprintf(stderr, "Entrada inválida.\n");
                return 1;
            }
            clear_line();
            continue;
        }

        Castillo c;
        if (lectura_archivo(archivo, &c) != 0) {
            printf("No se pudo procesar el archivo '%s'.\n", archivo);
            printf("¿Desea intentar con otro castillo? (y/n): ");
            if (scanf(" %c", &respuesta) != 1) {
                fprintf(stderr, "Entrada inválida.\n");
                return 1;
            }
            clear_line();
            continue;
        }

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

        if (inicio_id == -1 || salida_id == -1) {
            printf("Error: no se pudo encontrar el nodo inicial o final.\n");
            destroy(&g);
            printf("¿Desea procesar otro castillo? (y/n): ");
            if (scanf(" %c", &respuesta) != 1) {
                fprintf(stderr, "Entrada inválida.\n");
                return 1;
            }
            clear_line();
            continue;
        }

        int vidas;
        printf("Ingrese vidas iniciales: ");
        while (scanf("%d", &vidas) != 1) {
            fprintf(stderr, "Entrada inválida. Ingrese un entero: ");
            clear_line();
        }
        clear_line(); // limpia el resto de la línea por si quedó algo

        printf("El grafo tiene %lld aristas en total.\n", edge_count(&g));
        int total_vecinos = 0;
        for (int i = 0; i < g.n_nodes; ++i) total_vecinos += g.nodes[i].count;
        printf("Promedio de vecinos por nodo: %.2f\n", (float)total_vecinos / g.n_nodes);

        bfs_camino(&g, inicio_id, salida_id, vidas);

        destroy(&g);

        printf("\n¿Desea procesar otro castillo? (y/n): ");
        if (scanf(" %c", &respuesta) != 1) {
            fprintf(stderr, "Entrada inválida.\n");
            return 1;
        }
        clear_line();

    } while (respuesta == 'y' || respuesta == 'Y');

    printf("Programa finalizado.\n");
    return 0;
}
