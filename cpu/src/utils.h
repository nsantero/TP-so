#ifndef UTILS_H
#define UTILS_H

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
#include <configs-cpu.h>

typedef struct {
    op_code tipo_instruccion;
    char* operando1;        
    char* operando2; 
    char* operando3; 
    char* operando4; 
    char* operando5;
    int operandoNumero;
} t_instruccion;

extern int memoria_fd;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern char* server_name_dispatch;
extern char* server_name_interrupt;
extern int interrumpir;
extern t_instruccion instruccion;
extern t_instruccion instruccionActual;

void cargar_configuracion(char* archivo_configuracion);
int server_escuchar(int fd_cpu_interrupt, int fd_cpu_dispatch);
void procesar_conexion_interrupt(void* void_args);
void procesar_conexion_dispatch(void* void_args);
void* escucharInterrupciones();

void mandarPaqueteaKernel(op_code codigoDeOperacion);
t_paquete * paqueteProceso(op_code codigoDeOperacion);
void mandarPaqueteaKernelGenerica(op_code codigoDeOperacion, char* nombreInterfaz, int tiempo);
void mandarPaqueteaKernelRead(op_code codigoDeOperacion, char* nombreInterfaz, char *registro1, char *registro2);

#endif