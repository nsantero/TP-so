#include "../../utils/include/utils.h"

void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion

	if (config == NULL) {
		perror("Archivo de configuracion no encontrado");
		exit(-1);
	}

	config_valores.ip_memoria =	config_get_string_value(config,    "IP_MEMORIA");
	config_valores.puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
	config_valores.puerto_escucha_dispatch = config_get_int_value(config,    "PUERTO_ESCUCHA_DISPATCH");
	config_valores.puerto_escucha_interrupt = config_get_int_value(config,    "PUERTO_ESCUCHA_INTERRUPT");
	config_valores.cant_enradas_tlb = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
	config_valores.algoritmo_tlb = onfig_get_string_value(config, "ALGORITMO_TLB");
	
	
}
