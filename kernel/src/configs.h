#ifndef CONFIGS_H
#define CONFIGS_H

#include <commons/config.h>
#include <commons/log.h>


typedef struct {
   char* PUERTO_ESCUCHA;
   char* IP_MEMORIA;
   char* PUERTO_MEMORIA;
   char* IP_CPU;
   char* PUERTO_CPU_DISPATCH; 
   char* PUERTO_CPU_INTERRUPT;
   char* ALGORITMO_PLANIFICACION;
   int QUANTUM;
   char** RECURSOS;
   char** INSTANCIARECURSOS;
   int GRADO_MULTIPROGRAMACION;
}t_configKernel;

extern t_configKernel configuracionKernel;

extern t_config* configKernel;

void armarConfig();

extern t_log* loggerKernel;

void iniciarLogger();

#endif