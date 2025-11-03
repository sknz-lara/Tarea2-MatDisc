#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "reader.h"

/* Cuenta si una línea contiene exactamente n enteros (soporta BOM UTF-8). */
static int leer_enteros(const char* linea, int n){
    int contador = 0;
    const char* ptr = linea;
    int temp;

    /* Saltar BOM si existe (0xEF,0xBB,0xBF) */
    if ((unsigned char)ptr[0] == 0xEF && (unsigned char)ptr[1] == 0xBB && (unsigned char)ptr[2] == 0xBF) {
        ptr += 3;
    }

    while (sscanf(ptr, "%d", &temp) == 1) {
        contador++;
        /* Avanzar hasta el siguiente espacio o fin */
        while (*ptr != ' ' && *ptr != '\0' && *ptr != '\n' && *ptr != '\r') ptr++;
        while (*ptr == ' ') ptr++;
        if (*ptr == '\0') break;
    }

    return (contador == n);
}

/* Coordenadas de celda (índices 0..dim-1) para Hrongan, salida, monstruos, portales, escaleras */
static bool coordenadas_validas_celda(const Castillo* c, int piso, int x, int y) {
    return c &&
           piso >= 0 && piso < c->pisos &&
           x    >= 0 && x    < c->ancho &&
           y    >= 0 && y    < c->alto;
}

/* Coordenadas de extremos de MURO: permite tocar el borde (0..ancho / 0..alto) */
static bool coordenadas_validas_extremo_muro(const Castillo* c, int piso, int x, int y) {
    return c &&
           piso >= 0 && piso < c->pisos &&
           x    >= 0 && x    <= c->ancho &&
           y    >= 0 && y    <= c->alto;
}

/* Valida que el muro sea horizontal o vertical y que cumpla 0<k<dim (consistente con add_wall). */
static bool muro_valido(const Castillo* c, int piso, int x, int y, int x2, int y2) {
    if (!coordenadas_validas_extremo_muro(c, piso, x, y) ||
        !coordenadas_validas_extremo_muro(c, piso, x2, y2)) {
        return false;
    }
    if (x == x2) {
        /* vertical: k = x; 0 < k < ancho */
        int k = x;
        return (k > 0 && k < c->ancho);
    } else if (y == y2) {
        /* horizontal: k = y; 0 < k < alto */
        int k = y;
        return (k > 0 && k < c->alto);
    }
    return false;
}

typedef enum { DIM, HRONGAN, SALIDA, MUROS, MONSTRUO, PORTAL, ESCALERA } Estado;

int lectura_archivo(const char* txt, Castillo* castillo) {
    if (!txt || !castillo) {
        printf("Error: parámetros inválidos a lectura_archivo.\n");
        return -1;
    }

    FILE* fp = fopen(txt, "r");
    if(!fp){
        printf("Error al abrir archivo %s\n", txt);
        return -1;
    }

    char buffer[MAX_LINEA];
    int lineas = 0;
    int estado = DIM;

    /* Inicializar contadores */
    castillo->num_muros = 0;
    castillo->num_escaleras = 0;
    castillo->num_monstruos = 0;
    castillo->num_portales = 0;

    while (fgets(buffer, sizeof(buffer), fp)) {
        lineas++;

        /* Normalizar fin de línea (como el original) */
        buffer[strcspn(buffer, "\n")] = 0;

        /* Saltar líneas vacías */
        if (strlen(buffer) == 0) continue;

        /* Cambios de sección */
        if (strcmp(buffer, "monstruos") == 0) { estado = MONSTRUO; continue; }
        if (strcmp(buffer, "portales")  == 0) { estado = PORTAL;   continue; }
        if (strcmp(buffer, "escaleras") == 0) { estado = ESCALERA; continue; }

        int piso, piso2, x, y, x2, y2, vidas;

        switch (estado) {
            case DIM: {
                if (!leer_enteros(buffer, 3) ||
                    sscanf(buffer, "%d %d %d", &piso, &x, &y) != 3) {
                    printf("Error en línea %d: la dimensión del castillo debe tener exactamente 3 enteros\n", lineas);
                    fclose(fp);
                    return -1;
                }
                if (piso <= 0 || x <= 0 || y <= 0) {
                    printf("Error en línea %d: dimensiones deben ser positivas (pisos, ancho, alto)\n", lineas);
                    fclose(fp);
                    return -1;
                }
                castillo->pisos = piso;
                castillo->ancho = x;
                castillo->alto  = y;
                estado = HRONGAN;
                break;
            }

            case HRONGAN: {
                if (!leer_enteros(buffer, 3) ||
                    sscanf(buffer, "%d %d %d", &piso, &x, &y) != 3 ||
                    !coordenadas_validas_celda(castillo, piso, x, y)) {
                    printf("Error en línea %d: la posición de Hrongan debe tener 3 enteros dentro de las dimensiones\n", lineas);
                    fclose(fp);
                    return -1;
                }
                castillo->hrongan.piso = piso;
                castillo->hrongan.x    = x;
                castillo->hrongan.y    = y;
                estado = SALIDA;
                break;
            }

            case SALIDA: {
                if (!leer_enteros(buffer, 3) ||
                    sscanf(buffer, "%d %d %d", &piso, &x, &y) != 3 ||
                    !coordenadas_validas_celda(castillo, piso, x, y)) {
                    printf("Error en línea %d: la salida debe tener 3 enteros dentro de las dimensiones\n", lineas);
                    fclose(fp);
                    return -1;
                }
                castillo->salida.piso = piso;
                castillo->salida.x    = x;
                castillo->salida.y    = y;
                estado = MUROS; /* de aquí en adelante, muros hasta que cambie la sección */
                break;
            }

            case MUROS: {
                if (!leer_enteros(buffer, 5) ||
                    sscanf(buffer, "%d %d %d %d %d", &piso, &x, &y, &x2, &y2) != 5 ||
                    !muro_valido(castillo, piso, x, y, x2, y2)) {
                    printf("Error en línea %d: cada muro son 5 enteros válidos; "
                           "debe ser vertical u horizontal, con 0<k<dim y extremos en el borde permitido.\n", lineas);
                    fclose(fp);
                    return -1;
                }
                if (castillo->num_muros >= MAX_MUROS) {
                    printf("Error: se excedió MAX_MUROS (%d)\n", MAX_MUROS);
                    fclose(fp);
                    return -1;
                }
                Muro* m = &castillo->muros[castillo->num_muros++];
                m->inicio.piso = piso; m->inicio.x = x; m->inicio.y = y;
                m->x_fin = x2; m->y_fin = y2;
                break;
            }

            case MONSTRUO: {
                if (!leer_enteros(buffer, 4) ||
                    sscanf(buffer, "%d %d %d %d", &piso, &x, &y, &vidas) != 4 ||
                    !coordenadas_validas_celda(castillo, piso, x, y)) {
                    printf("Error en línea %d: cada monstruo son 4 enteros válidos\n", lineas);
                    fclose(fp);
                    return -1;
                }
                if (castillo->num_monstruos >= MAX_MONSTRUOS) {
                    printf("Error: se excedió MAX_MONSTRUOS (%d)\n", MAX_MONSTRUOS);
                    fclose(fp);
                    return -1;
                }
                Monstruo* mons = &castillo->monstruos[castillo->num_monstruos++];
                mons->posicion_monstruo.piso = piso;
                mons->posicion_monstruo.x    = x;
                mons->posicion_monstruo.y    = y;
                mons->vidas = vidas;
                break;
            }

            case PORTAL: {
                if (!leer_enteros(buffer, 6) ||
                    sscanf(buffer, "%d %d %d %d %d %d", &piso, &x, &y, &piso2, &x2, &y2) != 6 ||
                    !coordenadas_validas_celda(castillo, piso, x, y) ||
                    !coordenadas_validas_celda(castillo, piso2, x2, y2)) {
                    printf("Error en línea %d: cada portal son 6 enteros válidos\n", lineas);
                    fclose(fp);
                    return -1;
                }
                if (castillo->num_portales >= MAX_PORTALES) {
                    printf("Error: se excedió MAX_PORTALES (%d)\n", MAX_PORTALES);
                    fclose(fp);
                    return -1;
                }
                Portal* p = &castillo->portales[castillo->num_portales++];
                p->portal_inicio.piso = piso;  p->portal_inicio.x = x;  p->portal_inicio.y = y;
                p->portal_fin.piso    = piso2; p->portal_fin.x    = x2; p->portal_fin.y    = y2;
                break;
            }

            case ESCALERA: {
                if (!leer_enteros(buffer, 3) ||
                    sscanf(buffer, "%d %d %d", &piso, &x, &y) != 3 ||
                    !coordenadas_validas_celda(castillo, piso, x, y) ||
                    !coordenadas_validas_celda(castillo, piso + 1, x, y)) {
                    printf("Error en línea %d: cada escalera son 3 enteros válidos y debe conectar con piso+1\n", lineas);
                    fclose(fp);
                    return -1;
                }
                if (castillo->num_escaleras >= MAX_ESCALERAS) {
                    printf("Error: se excedió MAX_ESCALERAS (%d)\n", MAX_ESCALERAS);
                    fclose(fp);
                    return -1;
                }
                Escalera* e = &castillo->escaleras[castillo->num_escaleras++];
                e->posicion_inferior.piso  = piso;     e->posicion_inferior.x  = x; e->posicion_inferior.y  = y;
                e->posicion_superior.piso  = piso + 1; e->posicion_superior.x  = x; e->posicion_superior.y  = y;
                break;
            }
        }
    }

    fclose(fp);
    return 0;
}
