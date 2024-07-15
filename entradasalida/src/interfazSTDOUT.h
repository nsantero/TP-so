#ifndef INTEFAZ_STDOUT_H
#define INTEFAZ_STDOUT_H

#include <peticionesEstructuras.h>
#include <tiposInterfaces.h>
#include <commons/config.h>
#include <entradasalida.h>
#include <pthread.h>
#include <semaphore.h>
#include <loggersIO.h>

extern Interfaz interfaz_STDOUT;
extern t_list * cola_procesos_STDOUT;
extern pthread_mutex_t mutex_cola_STDOUT;
extern sem_t sem_hay_en_cola_STDOUT;



Interfaz generarNuevaInterfazSTDOUT(char* nombre,t_config* configuracion);

void EJECUTAR_INTERFAZ_STDOUT();

void* manejo_interfaz_STDOUT();


#endif