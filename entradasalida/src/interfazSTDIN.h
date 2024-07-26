#ifndef INTEFAZ_STDIN_H
#define INTEFAZ_STDIN_H

#include <peticionesEstructuras.h>
#include <tiposInterfaces.h>
#include <commons/config.h>
#include <pthread.h>
#include <semaphore.h>
#include <shared.h>
#include <entradasalida.h>
#include <loggersIO.h>
#include <string.h>
#include <readline/readline.h>

extern Interfaz interfaz_STDIN;
extern t_list * cola_procesos_STDIN;
extern pthread_mutex_t mutex_cola_STDIN;
extern sem_t sem_hay_en_cola_STDIN;



Interfaz generarNuevaInterfazSTDIN(char* nombre,t_config* configuracion);

void EJECUTAR_INTERFAZ_STDIN();

void* manejo_interfaz_STDIN();


#endif