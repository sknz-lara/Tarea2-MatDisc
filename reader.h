#ifndef READER_H
#define READER_H

#include <stdbool.h>

#define MAX_LINEA     256
#define MAX_MUROS      50
#define MAX_MONSTRUOS  50
#define MAX_PORTALES   50
#define MAX_ESCALERAS  50

typedef struct { int piso, x, y; } Coordenada;

typedef struct { Coordenada inicio; int x_fin, y_fin; } Muro;

typedef struct { Coordenada posicion_monstruo; int vidas; } Monstruo;

typedef struct { Coordenada portal_inicio; Coordenada portal_fin; } Portal;

typedef struct { Coordenada posicion_inferior; Coordenada posicion_superior; } Escalera;

typedef struct {
    int pisos, ancho, alto;
    Coordenada hrongan, salida;
    Muro      muros[MAX_MUROS];        int num_muros;
    Monstruo  monstruos[MAX_MONSTRUOS]; int num_monstruos;
    Portal    portales[MAX_PORTALES];   int num_portales;
    Escalera  escaleras[MAX_ESCALERAS]; int num_escaleras;
} Castillo;

int lectura_archivo(const char* txt, Castillo* castillo);

#endif
