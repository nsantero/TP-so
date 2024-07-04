#include "../../kernel/src/kernel.h"

typedef struct {
    int PID;
    CPU_Registers cpuRegisters;
} Proceso;

Proceso recibirProcesoAEjecutar(Proceso proceso);