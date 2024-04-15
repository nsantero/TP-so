#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

typedef struct
{
	char* ip_memoria;
    int puerto_memoria; 		
    int puerto_escucha_dispatch; 		
    int puerto_escucha_interrupt; 		
    int cant_enradas_tlb; 			
    char* algoritmo_tlb; 			
   
} cpu_config;

cpu_config config_valores;





