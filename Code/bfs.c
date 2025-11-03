#include "bfs.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int nodo;
    int vidas;
} Estado;


bool bfs_camino(const Graph *g, int inicio_id, int salida_id, int vidas_iniciales) {
    // Validación básica: el grafo debe existir y las vidas ser positivas.
    if (!g || vidas_iniciales <= 0) {
        fprintf(stderr, "Error: vidas_iniciales debe ser positivo.\n");
        return false;
    }

    // Total de nodos y total de posibles estados (nodo, vidas)
    int n = g->n_nodes;
    int total_estados = n * (vidas_iniciales + 1);

    // Arreglo de visitados: marca qué estados ya se exploraron.
    int *visitado = calloc(total_estados, sizeof(int));

    // Arreglo de predecesores: para reconstruir el camino.
    int *prev = malloc(total_estados * sizeof(int));
    for (int i = 0; i < total_estados; ++i) prev[i] = -1;

    // Cola para el recorrido BFS (FIFO)
    Estado *queue = malloc(sizeof(Estado) * total_estados);
    int front = 0, back = 0; // índices de inicio y fin de la cola

    // Estado inicial: nodo de Hrognan con todas sus vidas.
    int idx0 = inicio_id * (vidas_iniciales + 1) + vidas_iniciales;
    queue[back++] = (Estado){inicio_id, vidas_iniciales};
    visitado[idx0] = 1;
    prev[idx0] = -2; // -2 marca el inicio del camino

    int found_idx = -1; // índice del estado final (si se encuentra)


    // **_____ Bucle principal del BFS _____**

    while (front < back) {
        Estado cur = queue[front++]; // desencolar el siguiente estado
        int node = cur.nodo;
        int vidas = cur.vidas;

        // Si llegamos a la salida y aún tenemos vidas, terminamos.
        if (node == salida_id && vidas > 0) {
            found_idx = node * (vidas_iniciales + 1) + vidas;
            break;
        }

        Node *actual = &g->nodes[node]; // nodo actual del grafo

        // Recorremos todos los vecinos (posiciones adyacentes)
        for (int i = 0; i < actual->count; ++i) {
            Node *vecino = actual->neighbors[i];
            int costo = vecino->enemy_cost; // vidas que se pierden si hay monstruo
            int nv = vidas - costo;         // vidas restantes tras avanzar

            if (nv <= 0) continue; // si muere al llegar, no se sigue

            // Convertimos (nodo, vidas_restantes) a un índice único.
            int idx = vecino->id * (vidas_iniciales + 1) + nv;

            // Si ese estado ya fue visitado, lo saltamos.
            if (visitado[idx]) continue;

            // Marcamos como visitado y guardamos el predecesor.
            visitado[idx] = 1;
            prev[idx] = node * (vidas_iniciales + 1) + vidas;

            // Encolamos el nuevo estado.
            queue[back++] = (Estado){vecino->id, nv};
        }
    }

    // Si no encontramos una salida válida (vidas > 0), informamos y terminamos.
    if (found_idx == -1) {
        printf("No hay escapatoria posible.\n");
        free(visitado);
        free(prev);
        free(queue);
        return false;
    }

    //Reconstruimos el camino de atras para adelante
    int path_len = 0;
    int tmp = found_idx;

    // Contamos cuantos pasos hay hasta llegar al inicio.
    while (prev[tmp] != -2) {
        path_len++;
        tmp = prev[tmp];
    }
    path_len++; // incluir el nodo inicial

    // Reservamos memoria para guardar el camino completo.
    int *path = malloc(sizeof(int) * path_len);
    tmp = found_idx;

    // Retrocedemos guardando los nodos del camino.
    for (int i = path_len - 1; i >= 0; --i) {
        path[i] = tmp / (vidas_iniciales + 1); // índice de nodo
        tmp = prev[tmp];
    }

    // Calculamos cuantas vidas quedaron al final.
    int vidas_finales = found_idx % (vidas_iniciales + 1);

    printf("Camino encontrado:\n");
    printf("Largo: %d movimientos\n", path_len - 1);
    printf("Vidas restantes: %d\n", vidas_finales);

    // Imprimimos la ruta en el formato pedido: [[p,x,y], ...]
    printf("Ruta: [");
    for (int i = 0; i < path_len; ++i) {
        Node n = g->nodes[path[i]];
        printf("[%d,%d,%d]", n.p, n.x, n.y);
        if (i + 1 < path_len) printf(", ");
    }
    printf("]\n");

    // Liberamos toda la memoria dinámica usada.
    free(visitado);
    free(prev);
    free(queue);
    free(path);

    return true;
}


//Funcion para encontrar el id del nodo
int find_node_id(const Graph *g, int p, int x, int y) {
    if (!g || !g->nodes) return -1;
    for (int i = 0; i < g->n_nodes; ++i) {
        if (g->nodes[i].p == p && g->nodes[i].x == x && g->nodes[i].y == y)
            return g->nodes[i].id;
    }
    return -1; // no encontrado
}
