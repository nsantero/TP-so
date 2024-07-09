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
extern int obtener_pid(void);


typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
} Estado;

typedef struct {
    uint32_t PC;
    uint8_t AX;
    uint8_t BX;
    uint8_t CX;
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t SI;
    uint32_t DI;
} CPU_Registers;

typedef struct {
    int PID; //id del proceso
    int pc; //direccionamiento
    int quantum;
    Estado estado; // duración del quantum 
    CPU_Registers cpuRegisters; // puntero a cantidad de registros de la cpu (el valor lo tendría la cpu)
    char* path;
} PCB;


PCB* crearPCB();

// Funciones para planificar

extern int leer_grado_multiprogramación();

// envios a cpu

//void paquete_crear_proceso(int PID_paquete, char* path_paquete, int pc_paquete);
void paquete_memoria_finalizar_proceso (int PID_paquete);
void paquete_memoria_crear_proceso(int PID_paquete, char* path_paquete);
PCB* cambiarARunning(t_list* lista_READY);

#endif