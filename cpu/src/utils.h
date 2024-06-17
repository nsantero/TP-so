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
#include <shared.h>

#include "../../kernel/src/kernel.h"

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
    char* operando3; 
    char* operando4; 
    char* operando5; 		//SUM AX BX
    //tengo que agregar porque puede haber mas, creo que hasta 5 
} t_instruccion;

extern int memoria_fd;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern cpu_config config_valores;
extern t_log* logger;
extern char* server_name_dispatch;
extern char* server_name_interrupt;
extern PCB pcbActual;
extern int interrumpir;
extern t_instruccion instruccionActual;


void cargar_configuracion(char* archivo_configuracion);
int server_escuchar(int fd_cpu_interrupt, int fd_cpu_dispatch);
static void procesar_conexion_interrupt(void* void_args);

static void procesar_conexion_dispatch(void* void_args);


void* escucharInterrupciones();