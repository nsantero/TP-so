#ifndef INTEFAZ_STDOUT_H
#define INTEFAZ_STDOUT_H

#include <stdint.h>
#include <tiposInterfaces.h>
#include <commons/config.h>
#include <pthread.h>
#include <semaphore.h>


extern Interfaz interfaz_STDOUT;
extern t_list * cola_procesos_STDOUT;
extern pthread_mutex_t mutex_cola_STDOUT;
extern sem_t sem_hay_en_cola_STDOUT;

typedef struct
{
    uint32_t direccion;
    uint8_t tamanio;
    int PID;
}Peticion_Interfaz_STDOUT;

Interfaz generarNuevaInterfazSTDOUT(char* nombre,char* pathConfiguracion);

void EJECUTAR_INTERFAZ_STDOUT();

void* manejo_interfaz_STDOUT();


#endif