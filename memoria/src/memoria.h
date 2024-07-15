#ifndef MEMORIA_H
#define MEMORIA_H

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include "../../shared/src/shared.h"
#include "configs.h"

// ESTURCTURAS
typedef struct
{	
    int pid_tabla_de_paginas;
    int numero_de_pagina;
    int numero_de_frame; 	
   
} tabla_de_paginas_proceso;

// Estructura del Proceso
typedef struct {

    int pid;
    char * path;
    t_list *instrucciones;
    t_list *tabla_de_paginas;

} Proceso;

typedef struct {
    int numeroInstruccion;
    char *instruccion;
} Instruccion;

typedef struct
{	   
    void* espacioUsuario; 
    int tam;
    int pagina_tam;
    int cantidad_frames;
    int* frames_libres;
    //t_list lista_frames;

} Memoria;
typedef struct 
{
    int pid;
    int pathLength;
    char* archivos_instrucciones;

} t_proceso;

//extern void* memoria_espacio;
//extern int cantidad_frames;

extern Memoria memoria;

extern t_bitarray* bitmap_frames;
//char* server_name;

int server_escuchar(int fd_memoria);

//static void procesar_conexion(void *void_args) ;

int recv_fetch_instruccion(int fd_modulo, int **pid, int **pc);

void leer_instruccion_por_pc_y_enviar(int *pid, int *pc, int fd) ;

char *armar_path_instruccion(int pid) ;

int calculoDeFrames(int memoria_tam, int pagina_tam);

void obtener_instruccion(char *path_consola, int pc, int fd) ;

void crearListas();

void inicializarMemoria();

void esquemaPaginacion();

extern t_list* lista_ProcesosActivos;

extern t_list* lista_frames;

extern t_list* tabla_de_paginas;

#endif