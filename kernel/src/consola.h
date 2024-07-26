#ifndef CONSOLA_H
#define CONSOLA_H
// INICIAR_PROCESO /home/utnso/Documents/tp-2024-1c-File-System-Fanatics/kernel/preliminares/script_solo_cpu.txt
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

void* detenerListanew();
void* detenerListaready();
void* detenerListaexec();
void* detenerListablocked();
void* detenerListaBlockedrecursos();
void* detenerListaReadyprioridad();



#endif // CONSOLA_H

