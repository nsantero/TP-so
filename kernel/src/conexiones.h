#ifndef CONEXIONES_H
#define CONEXIONES_H

#include <utils.h>
#include <planificadores.h>
#include <kernel.h>

extern int memoria_fd;
extern int cpu_dispatch_fd;
extern int cpu_interrupt_fd;
extern int server_fd;
extern char* server_name;

void* conexionesDispatch();
void inicializar_servidor();
static void procesar_conexion(void *void_args);
extern int server_escuchar(int fd_memoria);

#endif // CONEXIONES_H