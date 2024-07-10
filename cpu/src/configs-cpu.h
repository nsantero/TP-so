#ifndef CONFIGS_CPU_H
#define CONFIGS_CPU_H

#include <commons/config.h>
#include <commons/log.h>

typedef struct {

	char* IP_MEMORIA;
    char* PUERTO_MEMORIA; 		
    char* PUERTO_ESCUCHA_DISPATCH; 		
    char* PUERTO_ESCUCHA_INTERRUPT; 		
    int CANTIDAD_ENTRADAS_TLB; 			
    char* ALGORITMO_TLB; 	

}t_configCpu;

extern t_configCpu configuracionCpu;

extern t_config* configCpu;

void armarConfig();

extern t_log* loggerCpu;

void iniciarLogger();

#endif