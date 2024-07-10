#ifndef CPU_H
#define CPU_H

#include "../../kernel/src/kernel.h"

typedef struct {
    int PID;
    CPU_Registers cpuRegisters;
} Proceso;

extern Proceso procesoEjecutando;

Proceso recibirProcesoAEjecutar(Proceso proceso);

void* manejarClienteKernel(void *arg);
void* atenderPeticionesKernel();
void paquete_memoria_pedido_instruccion(int PID_paquete);
extern pthread_mutex_t mutexSocketKernel;
extern pthread_mutex_t mutexSocketCpu;

#endif