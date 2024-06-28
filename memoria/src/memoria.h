#ifndef MEMORIA_H
#define MEMORIA_H

#include "utils.h"
#include <commons/log.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include "../../shared/src/shared.h"

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
    int cantidad_paginas;
    paginas_proceso* paginas;

} Proceso;

typedef struct
{	   
    void* memoria_espacio; 
    int memoria_tam;
    char* pagina_tam;
    int cantidad_frames;
    char* frames_libres;
    t_list lista_frames;

} memoria;
typedef struct
{		
    char* PUERTO_ESCUCHA; 
    int TAM_MEMORIA;
    int TAM_PAGINA; 
    char* PATH_INSTRUCCIONES;
    int RETARDO_RESPUESTA; 		
   
} memoria_config;

typedef struct 
{
    int pid;
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

void* memoria_espacio;
extern int cantidad_frames;

extern memoria_config congifuracionMemoria;

extern t_config* configMemoria;

t_log* loggerMemoria;

char* server_name;

void cargar_configuracion();

int server_escuchar(int fd_memoria);

static void procesar_conexion(void *void_args) ;

int recv_fetch_instruccion(int fd_modulo, int **pid, int **pc);

void leer_instruccion_por_pc_y_enviar(int *pid, int *pc, int fd) ;

char *armar_path_instruccion(int pid) ;

void obtener_instruccion(char *path_consola, int pc, int fd) ;

void crearListas() ;

t_instruccion armar_instruccion(char* instruccion_leida);

t_list* lista_ProcesosActivos;

t_list* lista_frames;

t_list* lista_de_paginas_proceso;

#endif