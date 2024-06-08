#ifndef KERNEL_H
#define KERNEL_H

#include <utils.h>
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

extern char *linea;
extern char *path_script;
extern char *path_proceso;
extern char *pid;
extern char *valor_multiprogramacion;
extern char *archivo_configuracion;

extern kernel_config config_valores;
extern t_log* logger;
extern int memoria_fd;
extern int cpu_dispatch_fd;
extern int cpu_interrupt_fd;
extern int server_fd;
extern char* server_name;

void cargar_configuracion(char* archivo_configuracion);

static void procesar_conexion(void *void_args);

int leer_grado_multiprogramación();

void planificar_round_robin();

int server_escuchar(int fd_memoria);

void corto_plazo(void* arg);

bool permitePasarAREady();

int obtener_pid(void); 

void inicializarListas();
// PCB
typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
} Estado;

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

typedef struct {
    int PID; //id del proceso
    int pc; //direccionamiento
    int quantum;
    Estado estado; // duración del quantum 
    CPU_Registers* cpuRegisters; // puntero a cantidad de registros de la cpu (el valor lo tendría la cpu)
} PCB;

extern PCB t_pcb;

extern int pid_counter ; //contador del pid, lo inicializo en 1
extern int quantum ; 
extern char* algoritmo_planificacion;
extern t_list* cola_de_procesos;
PCB* crearPCB();
// LISTAS DE ESTADOS

    extern t_list* lista_NEW;
    extern t_list* lista_READY; 
    extern t_list* lista_EXIT;
    extern t_list* lista_BLOCKED;
    extern t_list* lista_RUNNING;

#endif