#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint-gcc.h>
//parametros de máximos 
#define MAX_LINEA 256
#define MAX_MUROS 50
#define MAX_MONSTRUOS 50
#define MAX_PORTALES 50
#define MAX_ESCALERAS 50

// ---------------- Estructuras de cada estado ----------------
// cordenada general portales
typedef struct { int piso, x, y;} Coordenada;

// piso y coordenadas inicio y fin de un muro
typedef struct {Coordenada inicio; int x_fin, y_fin;} Muro;

typedef struct {Coordenada posicion_monstruo; int vidas;} Monstruo;

typedef struct {Coordenada portal_inicio; Coordenada portal_fin;} Portal;

typedef struct {Coordenada posicion_inferior; Coordenada posicion_superior;} Escalera;

typedef struct {
    int pisos, ancho, alto; // primera linea
    Coordenada hrongan, salida; // segunda y tercera
    Muro muros[MAX_MUROS]; //cuarta
    // numero de muros que se escriben (o lineas de muros descritos):
    int num_muros; 
    Monstruo monstruos[MAX_MONSTRUOS]; //quinta linea
    // numero de monstruos que hay
    int num_monstruos; 
    Portal portales[MAX_PORTALES]; // sexta línea
    // portales que se escriben, si hay una linea recordar 
    // que ambas coordenadas son portales (entrada y salida a la vez)
    int num_portales;
    Escalera escaleras[MAX_ESCALERAS]; // ultima linea 
    // la escalera de la primera linea indica piso de comienzo solamente y coordenadas
    // se debe implementar que aparezca en el piso_escalera + 1 para el siguiente piso
    int num_escaleras;
} Castillo;

// Nueva versión de leer_enteros
// Nueva versión de leer_enteros
int leer_enteros(const char* linea, int n){
    int contador = 0;
    const char* ptr = linea;
    int temp;

    // Saltar BOM si existe (0xEF,0xBB,0xBF)
    if ((unsigned char)ptr[0] == 0xEF && (unsigned char)ptr[1] == 0xBB && (unsigned char)ptr[2] == 0xBF) {
        ptr += 3;
    }

    while (sscanf(ptr, "%d", &temp) == 1) {
        contador++;
        // Avanzar hasta el siguiente espacio
        while (*ptr != ' ' && *ptr != '\0' && *ptr != '\n') ptr++;
        while (*ptr == ' ') ptr++;
    }

    return (contador == n);
}

// Verificar que las coordenadas estén dentro de los límites del castillo
bool coordenadas_validas(const Castillo* castillo, int piso, int x, int y) {
    return piso >= 0 && piso <= castillo->pisos &&
           x >= 0 && x <= castillo->ancho &&
           y >= 0 && y <= castillo->alto;
}


typedef enum{ DIM, HRONGAN, SALIDA, MUROS, MONSTRUO, PORTAL, ESCALERA } Estado;

int lectura_archivo(const char* txt, Castillo* castillo) {
    FILE* fp = fopen(txt,"r");
    if(!fp){
        printf("Error al abrir archivo %s\n",txt);
        return -1;
    }
    // verificamos que 

    char buffer[MAX_LINEA];
    int lineas=0; // contador para lineas del archivo
    int estado=DIM;
    /**
     * Para el estado:
     * 0 dimensiones castillo - 1 coordenadas hrogan
     * 2 coordenadas salida   - 3 inicio y fin de muros
     * 4 posición de monstruos - 5 portales - 6 escaleras
     * 
     * Con esto, inicializamos unos contadores de la estructura del castillo
     */
    castillo->num_muros=0; castillo->num_escaleras=0;
    castillo->num_monstruos=0; castillo->num_portales=0;
    
    // siempre que hayan lineas por leer, se continúa
    while (fgets(buffer,sizeof(buffer),fp)){
        lineas++;
        //los saltos de linea se omiten
        buffer[strcspn(buffer,"\n")] = 0;
        // Saltar líneas vacías, se omitirán
        if (strlen(buffer) == 0) continue;
        // Detectar lineas monstruo, portales y escalera
        if (strcmp(buffer, "monstruos") == 0) {
            estado = MONSTRUO;
            continue;
        } else if (strcmp(buffer, "portales") == 0) {
            estado = PORTAL;
            continue;
        } else if (strcmp(buffer, "escaleras") == 0) {
            estado = ESCALERA;
            continue;
        }
        
        int piso,piso2, x, y, x2, y2, vidas;
        switch(estado){
        // seguimos los casos de estados para verificar que cada linea tenga el número de enteros exactos
            case DIM:{
                if (!leer_enteros(buffer,3) || sscanf(buffer, "%d %d %d", &piso, &x, &y) != 3) {
                    printf("Error en línea %d: La dimensión del castillo deben tener exactamente 3 enteros\n", lineas);
                    fclose(fp);
                    return -1;
                }
                castillo->pisos=piso; castillo->ancho=x; castillo->alto=y;                
                estado=HRONGAN;
                break;
            }
            case HRONGAN:{
                if (!leer_enteros(buffer,3) || sscanf(buffer, "%d %d %d", &piso, &x, &y) != 3
                    || !coordenadas_validas(castillo, piso, x, y)){
                    printf("Error en línea %d: La posición de Hrongan deben tener exactamente 3 enteros dentro de las dimensiones del castillo\n", lineas);
                    fclose(fp);
                    return -1;
                }
                castillo->hrongan.piso=piso; castillo->hrongan.x=x, castillo->hrongan.y=y;
                estado=SALIDA;
                break;
            }
            case SALIDA:{ // Posición de la salida
                if (!leer_enteros(buffer,3) || sscanf(buffer, "%d %d %d", &piso, &x, &y) != 3
                    || !coordenadas_validas(castillo, piso, x, y)) {
                    printf("Error en línea %d: La salida deben tener exactamente 3 enteros y estar dentro de las dimensiones del castillo\n", lineas);
                    fclose(fp);
                    return -1;
                }
                castillo->salida.piso = piso; castillo->salida.x = x; castillo->salida.y = y;
                estado = MUROS;
                break;
            }
            case MUROS:{
                if (!leer_enteros(buffer,5) || sscanf(buffer, "%d %d %d %d %d", &piso, &x, &y, &x2, &y2) != 5 ||
                    !coordenadas_validas(castillo, piso, x, y) ||
                    !coordenadas_validas(castillo, piso, x2, y2)) {
                    printf("Error en línea %d: Los muros se definen con 5 enteros y dimensiones dentro de las del castillo\n", lineas);
                    fclose(fp);
                    return -1;
                }
                Muro* m=&castillo->muros[castillo->num_muros++];
                m->inicio.piso = piso; m->inicio.x = x; m->inicio.y = y;
                m->x_fin = x2; m->y_fin = y2;
                break;
            }
            case MONSTRUO:{
                if (!leer_enteros(buffer,4) || sscanf(buffer, "%d %d %d %d", &piso, &x, &y, &vidas) != 4 ||
                    !coordenadas_validas(castillo, piso, x, y)) {
                    printf("Error en línea %d: Los muros se definen con 4 enteros\n", lineas);
                    fclose(fp);
                    return -1;
                }
                Monstruo* mons=&castillo->monstruos[castillo->num_monstruos++];
                mons->posicion_monstruo.piso = piso;
                mons->posicion_monstruo.x = x;
                mons->posicion_monstruo.y = y;
                mons->vidas = vidas;
                break;
            }
            case PORTAL: {
                
                if (!leer_enteros(buffer,6) || sscanf(buffer, "%d %d %d %d %d %d", &piso, &x, &y, &piso2, &x2, &y2) != 6 ||
                    !coordenadas_validas(castillo, piso, x, y) || !coordenadas_validas(castillo, piso2, x2, y2)) {
                    printf("Error en línea %d: Los portales se definen con 6 enteros\n", lineas);
                    fclose(fp);
                    return -1;
                }
                Portal* p=&castillo->portales[castillo->num_portales++];
                p->portal_inicio.piso = piso; p->portal_inicio.x = x; p->portal_inicio.y = y;
                p->portal_fin.piso = piso2; p->portal_fin.x = x2; p->portal_fin.y = y2;
                break;
            }
            case ESCALERA: {
                
                if (!leer_enteros(buffer,3) ||sscanf(buffer, "%d %d %d", &piso, &x, &y) != 3 ||
                    !coordenadas_validas(castillo, piso, x, y) || !coordenadas_validas(castillo, piso + 1, x, y)) {
                    printf("Error en línea %d: Las escaleras se definen con 3 enteros\n", lineas);
                    fclose(fp);
                    return -1;
                }
                Escalera* e=&castillo->escaleras[castillo->num_escaleras++];
                e->posicion_inferior.piso = piso; e->posicion_inferior.x = x; e->posicion_inferior.y = y;
                e->posicion_superior.piso = piso + 1; e->posicion_superior.x = x; e->posicion_superior.y = y;
                break;
            }
        }
    } 
    fclose(fp);
    return 0;  
}

int main(int argc, char **argv) {
    Castillo c;
    if (argc != 2) {
        fprintf(stderr, "Uso: %s archivo\n", argv[0]);
        return -1;
    }

    char* txt = (argv[1]); //archivo
    //lectura_archivo("C:\\Users\\cuchu\\OneDrive - Universidad de Concepción\\Informática\2025-2\\Mate_Discretas\\Tarea2-MatDisc\\Castles/"+txt, &c);
    lectura_archivo(txt, &c);
    printf("Castillo leido: %d pisos, ancho %d y alto %d\n", c.pisos, c.ancho, c.alto);
    printf("Hrongan: piso %d, x %d, y %d\n", c.hrongan.piso, c.hrongan.x, c.hrongan.y);
    printf("Salida: piso %d, x %d, y %d\n", c.salida.piso, c.salida.x, c.salida.y);
    printf("Número de muros: %d\n", c.num_muros);
    printf("Número de monstruos: %d\n", c.num_monstruos);
    printf("Número de portales: %d\n", c.num_portales);
    printf("Número de escaleras: %d\n", c.num_escaleras);
    return 0;
}