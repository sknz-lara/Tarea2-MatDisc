#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINEA 256
#define MAX_MUROS 100
#define MAX_MONSTRUOS 50
#define MAX_PORTALES 50
#define MAX_ESCALERAS 50

//-------------------------------- structures ----------------------------------

typedef struct {
    int piso;
    int x;
    int y;
} Coordenada;

typedef struct {
    int piso;
    int x_inicio;
    int y_inicio;
    int x_fin;
    int y_fin;
} Muro;

typedef struct {
    int piso;
    int x;
    int y;
    int vidas;
} Monstruo;

typedef struct {
    Coordenada portal1;
    Coordenada portal2;
} Portal;

typedef struct {
    Coordenada posicion;
} Escalera;

typedef struct {
    int pisos;
    int ancho;
    int alto;
    Coordenada hrognan;
    Coordenada salida;
    Muro muros[MAX_MUROS];
    int num_muros;
    Monstruo monstruos[MAX_MONSTRUOS];
    int num_monstruos;
    Portal portales[MAX_PORTALES];
    int num_portales;
    Escalera escaleras[MAX_ESCALERAS];
    int num_escaleras;
} Castillo;

//--------------------------- rango / validation helpers ------------------------

int validar_coord(const Castillo *c, int piso, int x, int y, int linea) {
    if (piso < 0 || piso >= c->pisos) {
        printf("Error en linea %d: piso %d fuera de rango [0..%d)\n",
               linea, piso, c->pisos);
        return 0;
    }
    if (x < 0 || x >= c->ancho) {
        printf("Error en linea %d: x %d fuera de rango [0..%d)\n",
               linea, x, c->ancho);
        return 0;
    }
    if (y < 0 || y >= c->alto) {
        printf("Error en linea %d: y %d fuera de rango [0..%d)\n",
               linea, y, c->alto);
        return 0;
    }
    return 1;
}

//------------------------------- parse helpers --------------------------------

int parse_3(const char *l, int *p, int *x, int *y) {
    return sscanf(l, "%d %d %d", p,x,y) == 3;
}
int parse_4(const char *l, int *p, int *x, int *y, int *v) {
    return sscanf(l, "%d %d %d %d", p,x,y,v) == 4;
}
int parse_5(const char *l, int *p, int *x1, int *y1, int *x2, int *y2) {
    return sscanf(l, "%d %d %d %d %d", p,x1,y1,x2,y2) == 5;
}
int parse_6(const char *l, int *p1,int *x1,int *y1,int *p2,int *x2,int *y2){
    return sscanf(l,"%d %d %d %d %d %d",p1,x1,y1,p2,x2,y2)==6;
}

//--------------------------------- add helpers --------------------------------

int add_muro(Castillo *c,int p,int x1,int y1,int x2,int y2){
    if(!validar_coord(c,p,x1,y1,0))return 0;
    if(!validar_coord(c,p,x2,y2,0))return 0;
    if(c->num_muros >= MAX_MUROS) return 0;
    Muro *m = &c->muros[c->num_muros++];
    m->piso=p; m->x_inicio=x1; m->y_inicio=y1; m->x_fin=x2; m->y_fin=y2;
    return 1;
}

int add_monstruo(Castillo *c,int p,int x,int y,int v){
    if(!validar_coord(c,p,x,y,0))return 0;
    if(c->num_monstruos >= MAX_MONSTRUOS) return 0;
    Monstruo *m = &c->monstruos[c->num_monstruos++];
    m->piso=p; m->x=x; m->y=y; m->vidas=v;
    return 1;
}

int add_portal(Castillo *c,int p1,int x1,int y1,int p2,int x2,int y2){
    if(!validar_coord(c,p1,x1,y1,0))return 0;
    if(!validar_coord(c,p2,x2,y2,0))return 0;
    if(c->num_portales >= MAX_PORTALES) return 0;
    Portal *p = &c->portales[c->num_portales++];
    p->portal1.piso=p1; p->portal1.x=x1; p->portal1.y=y1;
    p->portal2.piso=p2; p->portal2.x=x2; p->portal2.y=y2;
    return 1;
}

int add_escalera(Castillo *c,int p,int x,int y){
    if(!validar_coord(c,p,x,y,0))return 0;
    if(c->num_escaleras >= MAX_ESCALERAS) return 0;
    Escalera *e = &c->escaleras[c->num_escaleras++];
    e->posicion.piso=p; e->posicion.x=x; e->posicion.y=y;
    return 1;
}

//------------------------------ lectura de archivo ----------------------------

enum { S_DIM=0, S_HROGNAN, S_SALIDA, S_MUROS, S_MONSTRUOS, S_PORTALES, S_ESCALERAS };

int lectura_txt(const char* nombre_archivo, Castillo* c) {
    FILE* f = fopen(nombre_archivo,"r");
    if(!f){ printf("Error al abrir archivo\n"); return -1; }

    c->num_muros=0; c->num_monstruos=0; c->num_portales=0; c->num_escaleras=0;

    char linea[MAX_LINEA];
    int estado=S_DIM;
    int num_linea=0;

    while(fgets(linea,sizeof(linea),f)){
        num_linea++;
        linea[strcspn(linea,"\n")]=0;
        if(strlen(linea)==0) continue;

        if(strcmp(linea,"monstruos")==0){ estado=S_MONSTRUOS; continue; }
        if(strcmp(linea,"portales")==0){  estado=S_PORTALES;  continue; }
        if(strcmp(linea,"escaleras")==0){ estado=S_ESCALERAS; continue; }

        int p,x,y,v,p2,x2,y2,x1,y1,xg,yg;

        switch(estado){
        case S_DIM:
            if(!parse_3(linea,&c->pisos,&c->ancho,&c->alto)){
                printf("Error formateo dimensiones en linea %d\n",num_linea);
                fclose(f); return -1;
            }
            estado=S_HROGNAN;
            break;

        case S_HROGNAN:
            if(!parse_3(linea,&p,&x,&y)||!validar_coord(c,p,x,y,num_linea)){
                fclose(f); return -1;
            }
            c->hrognan.piso=p; c->hrognan.x=x; c->hrognan.y=y;
            estado=S_SALIDA;
            break;

        case S_SALIDA:
            if(!parse_3(linea,&p,&x,&y)||!validar_coord(c,p,x,y,num_linea)){
                fclose(f); return -1;
            }
            c->salida.piso=p; c->salida.x=x; c->salida.y=y;
            estado=S_MUROS;
            break;

        case S_MUROS:
            if(!parse_5(linea,&p,&x,&y,&x2,&y2)||!add_muro(c,p,x,y,x2,y2)){
                printf("Error muro en linea %d\n",num_linea);
                fclose(f); return -1;
            }
            break;

        case S_MONSTRUOS:
            if(!parse_4(linea,&p,&x,&y,&v)||!add_monstruo(c,p,x,y,v)){
                printf("Error monstruo en linea %d\n",num_linea);
                fclose(f); return -1;
            }
            break;

        case S_PORTALES:
            if(!parse_6(linea,&p,&x,&y,&p2,&x2,&y2)||!add_portal(c,p,x,y,p2,x2,y2)){
                printf("Error portal en linea %d\n",num_linea);
                fclose(f); return -1;
            }
            break;

        case S_ESCALERAS:
            if(!parse_3(linea,&p,&x,&y)||!add_escalera(c,p,x,y)){
                printf("Error escalera en linea %d\n",num_linea);
                fclose(f); return -1;
            }
            break;
        }
    }

    fclose(f);
    return 0;
}

//----------------------------------- imprimir ---------------------------------

void imprimir_castillo(const Castillo* c) {
    printf("Dim %d pisos %dx%d\n",c->pisos,c->ancho,c->alto);
}

//------------------------------------ main ------------------------------------

int main(){
    Castillo c;
    if(lectura_txt("castillo.txt",&c)==0){
        printf("ok\n");
        imprimir_castillo(&c);
    }
    return 0;
}
