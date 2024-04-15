#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

typedef struct
{		
    int puerto_escucha; 
    char* ip_memoria;
    int puerto_memoria; 
    char* ip_cpu;
    int puerto_cpu_dispatch; 		
    int puerto_cpu_interrupt; 				
    char* algoritmo_planificacion; 	
    int quantum;
    t_list* recursos;
    t_list* instancias_recursos;
    int grado_multiprogramacion;
   
} kernel_config;

kernel_config config_valores;









