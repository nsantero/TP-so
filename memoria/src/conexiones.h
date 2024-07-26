#ifndef CONEXIONESMEMORIA_H
#define CONEXIONESMEMORIA_H

#include <../../shared/src/shared.h>
#include <memoria.h>

void liberarFrame(int frame);

void destroy_page_entry(void *element);

int cantidadFrameLibre();

int obtener_marco(int pid,int pagina);

int asignarFrameLibre();

Proceso* obtener_proceso(int pid_remover);

void enviar_resultado_instruccion_resize(op_code resultado,int socket_cliente);

void enviar_paquete_cpu_mov_in(op_code resultado,void* datos_leidos, int size_datos, int socketCliente);

void enviar_paquete_cpu_mov_out(op_code resultado, int socketCliente);

op_code actualizar_tam_proceso(int pid_a_cambiar,int tam_a_cambiar);

char* buscar_instruccion(int pid_a_buscar,int pc_a_buscar);

void paquete_cpu_envio_instruccion(int PID_paquete,int PC_paquete,int socket_cliente);

void enviar_paquete_cpu_marco(int marco_encontrado,int socketCliente);

void paquete_cpu_envio_tam_pagina(int socket_cliente);

void remover_proceso(int pid_remover);

void enviar_paquete_cpu_mov_out_prueba(op_code resultado,void* datos_leidos, int size_datos, int socketCliente);

void* manejarClienteCpu(void *arg);

void* atenderPeticionesCpu();

void* manejarClienteKernel(void *arg);

void* atenderPeticionesKernel();

void* atenderPeticionesEntradaSalida();

void* manejarClienteEntradaSalida(void *arg);

void cargarInstrucciones(Proceso *proceso, const char *path);

void destruirInstruccion(void *elemento);

void limpiarInstrucciones(Proceso *proceso);

extern int server_fd;

#endif