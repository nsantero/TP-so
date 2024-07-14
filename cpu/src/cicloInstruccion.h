#ifndef CICLOINSTRUCCION_H
#define CICLOINSTRUCCION_H

#include <utils.h>
#include <cpu.h>
#define MEM_SIZE 256
// Memoria ficticia para almacenar instrucciones
extern char memoria[MEM_SIZE][20]; // Cada instrucción tiene un tamaño máximo de 20 caracteres

extern char* instruccionRecibida;

char* fetch(Proceso *proceso);

void decode(char *instruccion);

void execute(CPU_Registers *cpu, t_instruccion instruccion_a_ejecutar);

void* ciclo_de_instruccion();

void recv_instruccion(int memoria_fd);

#endif // CICLOINSTRUCCION_H






