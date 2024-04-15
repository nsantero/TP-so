#include "../include/utils.h"


void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion

	if (config == NULL) {
		perror("Archivo de configuracion no encontrado");
		exit(-1);
	}

	config_valores.puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    config_valores.tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    config_valores.tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    config_valores.path_instrucciones = config_get_string_value(config,"PATH_INSTRUCCIONES");
    config_valores.retardo_respuesta = config_get_string_value(config,"RETARDO_RESPUESTA");
	
}

