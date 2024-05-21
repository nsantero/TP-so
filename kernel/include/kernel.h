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

// PCB

typedef struct {
    int PID; //id del proceso
    int pc; //direccionamiento
    int quantum;
    Estado estado; // duración del quantum 
    CPU_Registers* cpuRegisters; // puntero a cantidad de registros de la cpu (el valor lo tendría la cpu)
} PCB;

int pid_counter = 1; //contador del pid, lo inicializo en 1
int quantum; 
char* algoritmo_planificacion; 

t_list* cola_de_procesos;

typedef struct {
    char PC [5];
    char AX [2];
    char BX [2];
    char CX [2];
    char DX [2];
    char EAX [5];
    char EBX [5];
    char ECX [5];
    char EDX [5];
    char SI [5];
    char DI [5];
} CPU_Registers;

// LISTAS DE ESTADOS

t_list* lista_NEW;
t_list* lista_READY;
t_list* lista_EXIT;
t_list* lista_BLOCKED;
t_list* lista_RUNNING;

