#include <stdlib.h>
#include <stdio.h>
#include <string.h>//Preguntar si se puede incluir
#include "../include/entradasalida.h"


void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion

	if (config == NULL) {
		perror("Archivo de configuracion no encontrado");
		exit(-1);
	}

	config_valores.tipo_interfaz = config_get_string_value(config,"TIPO_INTERFAZ");
	config_valores.tiempo_unidad_trabajo = config_get_int_value(config,"TIEMPO_UNIDAD_TRABAJO");
	config_valores.ip_kernel= config_get_string_value(config,"IP_KERNEL");
	config_valores.puerto_kernel = config_get_string_value(config,"PUERTO_KERNEL"); 
    config_valores.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_valores.puerto_memoria= config_get_string_value(config,"PUERTO_MEMORIA");
	config_valores.path_base_dials = config_get_string_value(config,"PATH_BASE_DIALS");
    config_valores.block_size = config_get_int_value(config,"BLOCK_SIZE");
    config_valores.block_count = config_get_int_value(config,"BLOCK_COUNT");
    config_valores.retraso_compactacion = config_get_int_value(config, "RETRASO_COMPACTACION");


}


int main(int argc, char* argv[]) {


    logger = log_create("../kernel.log", "ENTRADASALIDA", true, LOG_LEVEL_INFO);
	log_info(logger, "Se creo el log!");

	char* archivo_configuracion =  argv[1];

	cargar_configuracion(archivo_configuracion);

	// Conecto entradasalida con kernel y memoria
	kernel_fd = crear_conexion(logger,"KERNEL",config_valores.ip_kernel,config_valores.puerto_kernel);
	log_info(logger, "Me conecte a kernel");
    memoria_fd = crear_conexion(logger,"MEMORIA",config_valores.ip_memoria,config_valores.puerto_memoria);
	log_info(logger, "Me conecte a memoria");

	iniciar_interfaz("ENTRADASALIDA","/home/utnso/tp-2024-1c-File-System-Fanatics/entradasalida/entradasalida.config");

	// envio mensajes
	enviar_mensaje("soy e/s", memoria_fd);
    enviar_mensaje("soy e/s", kernel_fd);


    return EXIT_SUCCESS;
}


void iniciar_interfaz(char nombre_interfaz, char* archivoConfig){

	char* tipoInterfaz = config_valores.tipo_interfaz;

	if(tipoInterfaz == "GENERICA"){
		IO_GEN_SLEEP();
	}
		else if (tipoInterfaz == "STDIN"){
			IO_STDIN_READ();
		}
			else if (tipoInterfaz == "STDOUT"){
		
				}
				else{//Es del tipo DialFS

	}

}

void IO_GEN_SLEEP(){

	sleep(config_valores.tiempo_unidad_trabajo);

}

