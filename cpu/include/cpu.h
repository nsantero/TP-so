#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <commons/collections/list.h>
#include <assert.h>
#include <pthread.h>
#include "../../shared/include/shared.h"
#include "instrucciones.h"

// ESTRUCTURAS //
typedef struct
{
    char* ip_memoria;
    char* puerto_memoria;
    char* puerto_escucha_dispatch;
    char* puerto_escucha_interrupt;
    int cant_enradas_tlb;
    char* algoritmo_tlb;
} cpu_config;

typedef struct
{
	char* instruccion;
    char* operando1; 		
    char* operando2; 		
    //tengo que agregar porque puede haber mas, creo que hasta 5 
} t_instruccion;

// VARIABLES //

int memoria_fd;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
cpu_config config_valores;
int socket_memoria;
t_log* cpu_log;
char* server_name_dispatch = "CPU_DISPATCH";
char* server_name_interrupt = "CPU_INTERRUPT";
int fd_cpu_dispatch;
int fd_cpu_interrupt;
bool recibio_interrupcion = false;

// FUNCIONES //
void cargar_configuracion_cpu(char* archivo_configuracion);
int server_escuchar(int fd_cpu_interrupt, int fd_cpu_dispatch);

static void procesar_conexion_interrupt(void* void_args);
t_instruccion recv_instruccion(int memoria_fd);

static void procesar_conexion_dispatch(void* void_args);
static void procesar_conexion_interrupt(void* void_args);

#endif