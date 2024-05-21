#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include "cpu.h"

char* server_name_dispatch = "CPU_DISPATCH";
char* server_name_interrupt = "CPU_INTERRUPT";
int fd_cpu_dispatch;
int fd_cpu_interrupt;

static void procesar_conexion_dispatch(void* void_args);

static void procesar_conexion_interrupt(void* void_args);

#endif