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

void *manejadorDeConsola();
void ejecutarScript(char* path);
//void iniciar_proceso(char* path);
void finalizar_proceso(int pid);
void detener_planificacion();
void iniciar_planificacion();
void modificar_multiprogramacion(int valor);
void ejecutarScript(char* path);
void procesarLinea(char* linea);


#endif // CONSOLA_H

