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
#include <shared.h>
#include "../../kernel/src/kernel.h"
#include "cpu.h"

extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern char* server_name_dispatch;
extern char* server_name_interrupt;
extern int interrumpir;

void cargar_configuracion(char* archivo_configuracion);
int server_escuchar(int fd_cpu_interrupt, int fd_cpu_dispatch);
static void procesar_conexion_interrupt(void* void_args);
static void procesar_conexion_dispatch(void* void_args);