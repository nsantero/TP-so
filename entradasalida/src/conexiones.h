#ifndef CONEXIONES_IO_H
#define CONEXIONES_IO_H

#include <../../shared/src/paquetes.h>
#include <../../shared/src/shared.h>
#include <commons/config.h>
#include <commons/string.h>
#include <peticionesEstructuras.h>
#include <loggersIO.h>

extern int kernel_fd;
extern int memoria_fd;


void enviarNuevaInterfazAKernel(t_config* configNueva,char* nombre);

void recibirPeticionDeIO_GEN();
void recibirPeticionDeIO_STDIN();
void recibirPeticionDeIO_STDOUT();
void recibirPeticionDeIO_DialFS();

void avisarErrorAKernel(char* nombre,int PID);
void terminoEjecucionInterfaz(char* nombre,int PID);

int enviarFragmentoAMemoria(op_code operacion, int pid, int marco, int desplazamiento,int size, void* datos);
void* solicitarFragmentoAMemoria(op_code operacion, int pid, int marco, int desplazamiento,int size);

#endif