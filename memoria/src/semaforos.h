#ifndef SEMAFOROSMEMORIA_H
#define SEMAFOROSMEMORIA_H

#include <../../shared/src/shared.h>
#include <semaphore.h>
extern pthread_mutex_t mutexSocketKernel;

extern pthread_mutex_t mutexSocketCpu;

extern pthread_mutex_t accesoAMemoria;

extern pthread_mutex_t listaProcesosActivos;

extern pthread_mutex_t actualizarLoggerMemoria;

extern pthread_mutex_t instruccionMutex;

extern pthread_mutex_t accesoBitArray;

#endif