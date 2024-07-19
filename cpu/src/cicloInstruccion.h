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

op_code recibir_confirmacion_memoria_resize();

int buscar_frame(int pagina);

void utilizacion_memoria(t_instruccion instruccion_memoria,int pid);

direccion_fisica *traduccion_mmu(char* datos,char* dl, int pid);

t_instruccion decode(char *instruccionDecodificar, int pid);

void execute(CPU_Registers *cpu, t_instruccion instruccion_a_ejecutar);

int execute2(t_instruccion instruccion_a_ejecutar,int pid);

void* ciclo_de_instruccion();

void recv_instruccion(int memoria_fd);

void mandarPaqueteaKernel(op_code codigoDeOperacion);
t_paquete * paqueteProceso(op_code codigoDeOperacion);
void mandarPaqueteaKernelGenerica(op_code codigoDeOperacion, char* nombreInterfaz, int tiempo);
void mandarPaqueteaKernelRead(op_code codigoDeOperacion, char* nombreInterfaz, char *registro1, char *registro2);

#endif // CICLOINSTRUCCION_H






