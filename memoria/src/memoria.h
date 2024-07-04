#ifndef MEMORIA_H
#define MEMORIA_H

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include "../../shared/src/shared.h"
#include "configs.h"

// ESTURCTURAS

typedef struct
{		
    int numeroDePagina;
    int numeroDeFrame; 	
   
} tabla_paginas_proceso;
typedef struct
{	
    int pid; 	
    t_list lista_de_paginas_proceso;
    
} paginas_proceso;

// Estructura del Proceso
typedef struct {

    int pid;
    char * path;
    //int cantidad_paginas;
    t_list *instrucciones;
    //paginas_proceso* paginas;

} Proceso;

typedef struct {
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

typedef struct
{
    char *opcode; // nombre instruccion
    char *var1;
    char *var2;
    char *var3;
    char *var4;
    char *var5;

} t_instruccion;

//extern void* memoria_espacio;
//extern int cantidad_frames;

extern Memoria memoria;

//char* server_name;

int server_escuchar(int fd_memoria);

static void procesar_conexion(void *void_args) ;

int recv_fetch_instruccion(int fd_modulo, int **pid, int **pc);

void leer_instruccion_por_pc_y_enviar(int *pid, int *pc, int fd) ;

char *armar_path_instruccion(int pid) ;

void obtener_instruccion(char *path_consola, int pc, int fd) ;

void crearListas() ;

t_instruccion armar_instruccion(char* instruccion_leida);

extern t_list* lista_ProcesosActivos;

extern t_list* lista_frames;

extern t_list* lista_de_paginas_proceso;

#endif