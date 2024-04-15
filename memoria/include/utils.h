#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>

typedef struct
{		
    int puerto_escucha; 
    int tam_pagina;
    int tam_memoria; 
    char* path_instrucciones;
    int retardo_respuesta; 		
   
   
} memoria_config;

memoria_config config_valores;










