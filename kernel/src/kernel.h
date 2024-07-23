#ifndef KERNEL_H
#define KERNEL_H

#include <utils.h>

void inicializarListas();
// LISTAS DE ESTADOS

    extern t_list* lista_NEW;
    extern t_list* lista_READY;
    extern t_list* lista_READYPRI;
    extern t_list* lista_EXIT;
    extern t_list* lista_BLOCKED;
    extern t_list* lista_BLOCKED_RECURSOS;
    extern t_list* lista_RUNNING;
    extern t_list *interfacesConectadas;

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
    uint32_t PID;
    int quantum;
    Estado estado;
    t_list* recursosEnUso;
    char* recursoBloqueante;
    CPU_Registers cpuRegisters;
} PCB;


PCB* crearPCB(char* path);
void eliminarProceso(PCB* proceso);
void actualizarProceso(PCB* procesoCPU, PCB* procesoKernel);

// Funciones para planificar

extern int leer_grado_multiprogramación();

// envios a cpu

//void paquete_crear_proceso(int PID_paquete, char* path_paquete, int pc_paquete);
void paquete_memoria_finalizar_proceso (int PID_paquete);
void paquete_memoria_crear_proceso(int PID_paquete, char* path_paquete);
PCB* buscarProcesoPID(uint32_t pid, t_list* lista);
PCB* buscarProcesoPIDSinRemover(uint32_t pid, t_list* lista);
void finalizarProceso(uint32_t pid);
void paquete_CPU_interrumpir_proceso_finalizado(int pid);
PCB* cambiarARunning(t_list* lista_READY);
extern int grado_multiprogramacion;

#endif