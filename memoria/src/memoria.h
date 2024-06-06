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


typedef struct
{		
    char* puerto_escucha; 
    int tam_pagina;
    int tam_memoria; 
    char* path_instrucciones;
    int retardo_respuesta; 		
   
   
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

memoria_config config_valores;
t_log* logger;
char* server_name;

void cargar_configuracion(char* archivo_configuracion);
int server_escuchar(int fd_memoria);
static void procesar_conexion(void *void_args) ;
int recv_fetch_instruccion(int fd_modulo, int **pid, int **pc);
void leer_instruccion_por_pc_y_enviar(int *pid, int *pc, int fd) ;
char *armar_path_instruccion(int pid) ;


void obtener_instruccion(char *path_consola, int pc, int fd) ;
t_instruccion armar_instruccion(char* instruccion_leida);
t_list* lista_procesos_activos;



#endif