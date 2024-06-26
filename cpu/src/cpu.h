#ifndef CPU_H_
#define CPU_H_

#include <commons/log.h>
#include "/home/utnso/tp-2024-1c-File-System-Fanatics/kernel/src/pcb.h"

typedef struct
{
	char* ip_memoria;
    char* puerto_memoria; 		
    char* puerto_escucha_dispatch; 		
    char* puerto_escucha_interrupt; 		
    int cant_entradas_tlb; 			
    char* algoritmo_tlb; 			
   
} cpu_config;

typedef struct
{
	char* instruccion;
    char* operando1; 		
    char* operando2; 
    char* operando3; 
    char* operando4; 
    char* operando5; 		//SUM AX BX
    //tengo que agregar porque puede haber mas, creo que hasta 5 
} t_instruccion;

// Funciones

void* escucharInterrupciones();

// Variables
extern cpu_config config_valores;
extern t_log* logger;
extern int memoria_fd;
extern t_instruccion instruccionActual;

#endif // CPU_H_