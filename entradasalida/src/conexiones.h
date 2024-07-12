#ifndef CONEXIONES_IO_H
#define CONEXIONES_IO_H

#include <../../shared/src/paquetes.h>
#include <../../shared/src/shared.h>
#include <commons/config.h>
#include <commons/string.h>
#include <peticionesEstructuras.h>

extern int kernel_fd;
extern int memoria_fd;


void enviarNuevaInterfazAKernel(t_config* configNueva,char* nombre);



#endif