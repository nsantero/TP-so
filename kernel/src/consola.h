#ifndef CONSOLA_H
#define CONSOLA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <kernel.h>
#include <mostrarProcesos.h>
#include <planificadores.h>
#include <configs.h>

void *manejadorDeConsola();
void ejecutarScript(char* path);
//void iniciar_proceso(char* path);
void finalizar_proceso(int pid);
void detener_planificacion();
void iniciar_planificacion();
void ejecutarScript(char* path);
void modificar_grado_multiprogramacion(int valor);
void procesarLinea(char* linea,  const char* scriptBasePath);
char* construirPathCompleto(char* argumento);

void liberarPCB(PCB* pcb);
void liberarTodasLasListas();
void terminarKernel();

void detener_cola_new(void* arg);
void detener_cola_ready(void* arg);
void detener_cola_exec(void* arg);
void detener_cola_blocked(void* arg);
void detener_cola_blocked_recursos(void* arg);
void detener_cola_blocked_prioridad(void* arg);



#endif // CONSOLA_H

