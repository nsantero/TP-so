#ifndef CICLOINSTRUCCION_H
#define CICLOINSTRUCCION_H

#include <utils.h>
#include <cpu.h>

// Memoria ficticia para almacenar instrucciones
char memoria[MEM_SIZE][20]; // Cada instrucción tiene un tamaño máximo de 20 caracteres

void fetch(CPU_Registers *cpu, char *instruccion);

const char* decode(const char *instruccion);

void execute(CPU_Registers *cpu, const char *instruccion);

void checkInterrupts(CPU_Registers *cpu);

void ciclo_de_instruccion(CPU_Registers *cpu);

void recv_instruccion(int memoria_fd);


#endif // CICLOINSTRUCCION_H






