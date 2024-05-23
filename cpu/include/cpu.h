#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <commons/collections/list.h>
#include <assert.h>
#include <pthread.h>
#include "../../shared/include/shared.h"
#include "config.h"
#include "utils.h"
#include "conexiones.h"

int memoria_fd;
t_log* logger;

void cargar_configuracion(char* archivo_configuracion);
int server_escuchar(int fd_cpu_interrupt, int fd_cpu_dispatch);
bool recibio_interrupcion = false;

#endif