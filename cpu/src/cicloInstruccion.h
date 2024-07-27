#ifndef CICLOINSTRUCCION_H
#define CICLOINSTRUCCION_H

#include <utils.h>
#include <cpu.h>
#include <instrucciones.h>
#define MEM_SIZE 256
// Memoria ficticia para almacenar instrucciones
extern char memoria[MEM_SIZE][20]; // Cada instrucción tiene un tamaño máximo de 20 caracteres

extern char* instruccionRecibida;

char* fetch(Proceso *proceso);

extern int instante_modificacion_tlb;

op_code recibir_confirmacion_memoria_resize();

int valorDelRegistro(char *dl,CPU_Registers registros);

uint32_t leerValorDelRegistro(char *dl,CPU_Registers registros);

uint8_t leerValorDelRegistro_8(char *dl,CPU_Registers registros);

int calculo_cantiad_paginas(uint32_t dl, int pid, int desplazamiento,int size_dato);

int recibir_marco_memoria();

int buscar_frame(int pagina,int pid);

int obtener_frame_en_tlb(int pid, int pagina);

void utilizacion_memoria(t_instruccion instruccion_memoria,int pid);

uint32_t recibir_leer_memoria_mov_in();

op_code recibir_confirmacion_memoria_mov_out();

void* recibir_confirmacion_memoria_mov_in();

void algoritmoFIFO(int pid,int marco_memoria,int pagina);

void algoritmoLRU(int pid,int marco_memoria,int pagina);

void agregar_marco_tlb(int pid,int marco_memoria,int pagina);

op_code buscar_en_tlb(int pid, int pagina);

direccion_fisica *traduccion_mmu(uint32_t dl, int pid);

uint32_t leerValorDelRegistro(char *dl,CPU_Registers registros);

t_instruccion decode(char *instruccionDecodificar, int pid);

void execute(CPU_Registers *cpu, t_instruccion instruccion_a_ejecutar);

int execute2(t_instruccion instruccion_a_ejecutar,int pid);

void* ciclo_de_instruccion();

void recv_instruccion(int memoria_fd);



#endif // CICLOINSTRUCCION_H






