#include "../../kernel/src/kernel.h"

typedef struct {
    int PID;
    CPU_Registers cpuRegisters;
} Proceso;

Proceso recibirProcesoAEjecutar(Proceso proceso);

void* manejarClienteKernel(void *arg);
void* atenderPeticionesKernel();
void paquete_memoria_pedido_instruccion(int PID_paquete);
extern pthread_mutex_t mutexSocketKernel;
extern pthread_mutex_t mutexSocketCpu;