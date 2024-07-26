#ifndef SEMAFOROSMEMORIA_H
#define SEMAFOROSMEMORIA_H

#include <../../shared/src/shared.h>
extern pthread_mutex_t mutexSocketKernel;
extern pthread_mutex_t mutexSocketCpu;

extern pthread_mutex_t accesoAMemoria;
extern pthread_mutex_t listaProcesosActivos;
extern pthread_mutex_t actualizarLoggerMemoria;

#endif