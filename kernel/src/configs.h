#ifndef CONFIGS_H
#define CONFIGS_H

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
   char* PUERTO_ESCUCHA;
   char* IP_MEMORIA;
   char* PUERTO_MEMORIA;
   char* IP_CPU;
   char* PUERTO_CPU_DISPATCH; 
   char* PUERTO_CPU_INTERRUPT;
   char* ALGORITMO_PLANIFICACION;
   int QUANTUM;
   t_list* RECURSOS;
   t_list* INSTANCIAS_RECURSOS;
   int GRADO_MULTIPROGRAMACION;
}t_configKernel;

typedef struct {
   char* nombre_recurso;
   int cantidad_instancias;
} t_recurso;

extern t_configKernel configuracionKernel;

extern t_config* configKernel;

void armarConfig();

extern t_log* loggerKernel;

void iniciarLogger();

void inicializarRecursos(t_configKernel* config, t_config* configFile);
void liberarRecursos(t_configKernel* config);
void imprimirConfig(t_configKernel* config);
void limpiarConfig();

#endif