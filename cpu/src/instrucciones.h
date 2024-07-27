#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include "../../kernel/src/kernel.h"
#include <cicloInstruccion.h>

void ejecutar_set(CPU_Registers *registros, char* registro, uint32_t valor);
void ejecutar_sum(CPU_Registers *cpu, char* destino, char* origen);
void ejecutar_sub(CPU_Registers *cpu, char* destino, char* origen);
void ejecutar_jnz(CPU_Registers *cpu, char* registro, uint32_t nueva_instruccion);
int ejecutar_wait(Proceso *procesoActual, char* recurso);
void paquete_solicitud_wait(char* recurso);
int recibir_resultado_recursos();
void paquete_kernel_envio_recurso_signal(char* recurso);
void paquete_kernel_envio_recurso_wait( char* recurso);
int ejecutar_signal(Proceso *procesoActual, char* recurso);
void ejecutarCopyString(Proceso *procesoEjecutando, int sizeDato);


#endif