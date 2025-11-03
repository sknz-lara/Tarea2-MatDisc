#ifndef BFS_H
#define BFS_H

#include "castle.h"
#include <stdbool.h>

bool bfs_camino(const Graph *g, int inicio_id, int salida_id, int vidas_iniciales);

#endif
