#include "utils.h"
#include "../../shared/include/shared.h"
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

typedef struct
{		
    char* puerto_escucha; 
    char* ip_memoria;
    char* puerto_memoria; 
    char* ip_cpu;
    char* puerto_cpu_dispatch; 		
    char* puerto_cpu_interrupt; 				
    char* algoritmo_planificacion; 	
    int quantum;
    char** recursos;
    char** instancias_recursos;
    int grado_multiprogramacion;
   
} kernel_config;

kernel_config config_valores;
t_log* logger;
int memoria_fd;
int cpu_dispatch_fd;
int cpu_interrupt_fd;
int server_fd;
char* server_name = "kernel";


void cargar_configuracion(char* archivo_configuracion);
static void procesar_conexion(void *void_args);
int server_escuchar(int fd_memoria);
