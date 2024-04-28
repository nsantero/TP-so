#include "../include/memoria.h"

void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion

	if (config == NULL) {
		perror("Archivo de configuracion no encontrado");
		exit(-1);
	}

	config_valores.puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    config_valores.tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    config_valores.tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    config_valores.path_instrucciones = config_get_string_value(config,"PATH_INSTRUCCIONES");
    config_valores.retardo_respuesta = config_get_int_value(config,"RETARDO_RESPUESTA");
	
}



int main(int argc, char *argv[])
{

    logger = log_create("./log/memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	log_info(logger, "Se creo el log!");

	cargar_configuracion("/home/utnso/tp-2024-1c-File-System-Fanatics/memoria/memoria.config");

	int server_fd = iniciar_servidor(logger, server_name ,IP, config_valores.puerto_escucha);  //cambiar variable global
	log_info(logger, "Servidor listo para recibir al cliente");

	// espero a kernel y cpu
	while(server_escuchar(server_fd));
	
	

    return 0;
}

////////////////////////////////////////////////////////// PROCESO CONEXION //////////////////////////////////////////////////////////

static void procesar_conexion(void *void_args) {
	int *args = (int*) void_args;
	int cliente_socket = *args;
	op_code cop;
    
	while (cliente_socket != -1) {
		if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
			log_info(logger, ANSI_COLOR_BLUE"El cliente se desconecto de %s server", server_name);
			return;
		}
		switch (cop) {
		case MENSAJE:{

			char* mensaje = recibir_mensaje(cliente_socket);
            log_info(logger, ANSI_COLOR_YELLOW"Me llegó el mensaje: %s", mensaje);
                
            free(mensaje); // Liberar la memoria del mensaje recibido

			break;
            }
		case PAQUETE:

			///// IMPLEMENTAR
			
			break;
		
        default:
            printf("Error al recibir  %d \n", cop);
            break;
	return;
        }
    }
}

int server_escuchar(int fd_memoria) {
	server_name = "Memoria";
	int cliente_socket = esperar_cliente(logger, server_name, fd_memoria);

	if (cliente_socket != -1) {
		pthread_t hilo;
		pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) &cliente_socket);
		pthread_detach(hilo);
		return 1;
	}

	return 0;
}



