#ifndef CONEXIONESMEMORIA_H
#define CONEXIONESMEMORIA_H

#include <../../shared/src/shared.h>
#include <memoria.h>

int cantidadFrameLibre();

int asignarFrameLibre();

void enviar_resultado_instruccion_resize(op_code resultado,int socket_cliente);

op_code actualizar_tam_proceso(int pid_a_cambiar,int tam_a_cambiar);

char* buscar_instruccion(int pid_a_buscar,int pc_a_buscar);

void paquete_cpu_envio_instruccion(int PID_paquete,int PC_paquete,int socket_cliente);

void paquete_cpu_envio_tam_pagina(int socket_cliente);

void* manejarClienteCpu(void *arg);

void* atenderPeticionesCpu();

void* manejarClienteKernel(void *arg);

void* atenderPeticionesKernel();

void cargarInstrucciones(Proceso *proceso, const char *path);

extern int server_fd;

#endif