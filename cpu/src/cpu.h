#ifndef CPU_H
#define CPU_H

#include "../../kernel/src/kernel.h"
typedef struct {
    int PID;
    CPU_Registers cpuRegisters;
} Proceso;

extern int tam_pagina;

extern Proceso *procesoEjecutando;

Proceso recibirProcesoAEjecutar(Proceso proceso);

void* manejarClienteKernel(void *arg);

void* atenderPeticionesKernel();

int pedir_tam_pagina_memoria(); 

void paquete_memoria_pedido_tam_pagina();

void paquete_memoria_pedido_instruccion(int PID_paquete,int PC_paquete);

extern pthread_mutex_t mutexSocketKernel;

extern pthread_mutex_t mutexSocketCpu;

#endif