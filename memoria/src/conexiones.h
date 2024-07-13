#ifndef CONEXIONESMEMORIA_H
#define CONEXIONESMEMORIA_H

#include <../../shared/src/shared.h>
#include "memoria.h"

void* atenderPeticionesKernel();

void* manejarClienteKernel(void *arg);

void* atenderPeticionesCpu();

void* manejarClienteCpu(void *arg);

void cargarInstrucciones(Proceso *proceso, const char *path);
char* buscar_instruccion(int pid_a_buscar,int pc_a_buscar);
void paquete_cpu_envio_instruccion(int PID_paquete,int PC_paquete,int socket_cliente);

extern int server_fd;
#endif