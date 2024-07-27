#ifndef LOGGERSIO_H
#define LOGGERSIO_H

#include <commons/log.h>
#include <commons/string.h>
#include <pathDeConfiguracionesIO.h>
#include <semaphore.h>
#include <pthread.h>

extern t_log* loggerIO;
extern pthread_mutex_t mutex_logger;

void inicializarLogger();

void inicializarLoggerDeInterfaz(char* nombreDeInterfaz);

void cerrarLogger();

#endif