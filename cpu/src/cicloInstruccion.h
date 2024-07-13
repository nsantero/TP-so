#ifndef CICLOINSTRUCCION_H
#define CICLOINSTRUCCION_H

#include <utils.h>
#include <cpu.h>
#define MEM_SIZE 256
// Memoria ficticia para almacenar instrucciones
extern char memoria[MEM_SIZE][20]; // Cada instrucción tiene un tamaño máximo de 20 caracteres

void fetch(Proceso *proceso);

const char* decode(char *instruccion);

void execute(CPU_Registers *cpu, const char *instruccion);

void checkInterrupts(CPU_Registers *cpu);

void* ciclo_de_instruccion();

void recv_instruccion(int memoria_fd);


#endif // CICLOINSTRUCCION_H






