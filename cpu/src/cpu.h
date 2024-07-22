#ifndef CPU_H
#define CPU_H

#include "../../kernel/src/kernel.h"
typedef struct {
    int PID;
    CPU_Registers cpuRegisters;
} Proceso;
typedef struct {
    int PID;
    int numero_frame;
    int desplazamiento;
} direccion_fisica;
typedef struct {
    int pid;
    int pagina;
    int marco;
    int ultima_modificacion;
} Registro_TLB;

extern t_list* lista_TLB;

extern int tam_pagina;

extern Proceso *procesoEjecutando;

extern int socketCliente;

Proceso recibirProcesoAEjecutar(Proceso proceso);

void enviar_paquete_mov_in_memoria(int pid, int marco, int desplazamiento);

void crearTLB();

void* manejarClienteKernel(void *arg);

void* atenderPeticionesKernel();

int pedir_tam_pagina_memoria(); 

void paquete_memoria_pedido_tam_pagina();

void paquete_memoria_marco(int pid,int pagina);

void enviar_paquete_mov_out_memoria( int pid, int marco, int desplazamiento,uint32_t datos);

void paquete_memoria_pedido_instruccion(int PID_paquete,int PC_paquete);

void paquete_memoria_resize(int PID_paquete,int tam_nuevo);

void* check_interrupts();

extern pthread_mutex_t mutexSocketKernel;
extern pthread_mutex_t mutexSocketCpu;
extern pthread_mutex_t mutexInterrupcion;
extern pthread_mutex_t mutexProcesoEjecutando;

#endif