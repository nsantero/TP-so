#ifndef KERNEL_H
#define KERNEL_H

#include <utils.h>


void inicializarListas();
// LISTAS DE ESTADOS

    extern t_list* lista_NEW;
    extern t_list* lista_READY; 
    extern t_list* lista_EXIT;
    extern t_list* lista_BLOCKED;
    extern t_list* lista_RUNNING;

// PCB

extern int pid_counter ; //contador del pid, lo inicializo en 1
extern int quantum ; 
extern char* algoritmo_planificacion;
extern int pidActual;
extern int generarPID();
extern PCB* crearPCB();
extern int obtener_pid(void); 

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

// CONSOLA
extern char *linea;
extern char *path_script;
extern char *path_proceso;
extern char *pid;
extern char *valor_multiprogramacion;
extern char *archivo_configuracion;

// Funciones para planificar

extern int leer_grado_multiprogramación();

#endif