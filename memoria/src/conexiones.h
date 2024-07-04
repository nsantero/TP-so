#ifndef CONEXIONESMEMORIA_H
#define CONEXIONESMEMORIA_H

#include <../../shared/src/shared.h>
#include "memoria.h"

void* atenderPeticionesKernel();

void* manejarClienteKernel(void *arg);

void cargarInstrucciones(Proceso *proceso, const char *path);

extern int server_fd;

#endif