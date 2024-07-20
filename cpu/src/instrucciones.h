#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include "../../kernel/src/kernel.h"
#include <cicloInstruccion.h>

void ejecutar_set(CPU_Registers *registros, const char* registro, uint8_t valor);
void ejecutar_sum(CPU_Registers *cpu, const char* destino, const char* origen);
void ejecutar_sub(CPU_Registers *cpu, const char* destino, const char* origen);
void ejecutar_jnz(CPU_Registers *cpu, const char* registro, uint32_t nueva_instruccion);
void ejecutar_wait(Proceso *procesoActual, const char* recurso);
void paquete_solicitud_wait(const char* recurso);
int recibir_resultado_recursos();

#endif