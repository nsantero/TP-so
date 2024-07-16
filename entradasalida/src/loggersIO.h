#ifndef LOGGERSIO_H
#define LOGGERSIO_H

#include <commons/log.h>
#include <commons/string.h>
#include <pathDeConfiguracionesIO.h>

extern t_log* loggerIO;

void inicializarLogger();

void inicializarLoggerDeInterfaz(char* nombreDeInterfaz);

void cerrarLogger();

#endif