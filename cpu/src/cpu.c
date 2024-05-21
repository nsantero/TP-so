#include "../include/cpu.h"

int main(int argc, char* argv[]) {
    
	logger = log_create("./log/cpu.log", "CPU", true, LOG_LEVEL_INFO);
	log_info(logger, "Se creo el log!");

	char* archivo_config = argv[1];
	
	cargar_configuracion(archivo_config);

	//me conecto a memoria
	memoria_fd = crear_conexion(logger,"CPU",config_valores.ip_memoria, config_valores.puerto_memoria);
	
	log_info(logger, "Me conecte a memoria!");

	//envio mensaje
    	enviar_mensaje("Hola, soy CPU!", memoria_fd);

	// levanto el servidor dispatch e interrupt
	fd_cpu_dispatch = iniciar_servidor(logger,server_name_dispatch ,IP, config_valores.puerto_escucha_dispatch);
	fd_cpu_interrupt = iniciar_servidor(logger, server_name_interrupt ,IP, config_valores.puerto_escucha_interrupt);
	log_info(logger, "Servidor listo para recibir al cliente");

	atender_dispatch(logger);

    return EXIT_SUCCESS;
}

int server_escuchar(int fd_cpu_interrupt, int fd_cpu_dispatch) {
    int socket_cliente_dispatch = esperar_cliente(logger, server_name_dispatch, fd_cpu_dispatch);
    int socket_cliente_interrupt = esperar_cliente(logger, server_name_interrupt, fd_cpu_interrupt);
	
    if (socket_cliente_interrupt != -1 && socket_cliente_dispatch != -1) {
        //hilo para servidor dispatch
        pthread_t hilo_dispatch;
		pthread_create(&hilo_dispatch, NULL, (void*) procesar_conexion_dispatch, (void*) &socket_cliente_dispatch);
		pthread_detach(hilo_dispatch);
        //hilo para servidor interrupt
        pthread_t hilo_interrupt;
		pthread_create(&hilo_interrupt, NULL, (void*) procesar_conexion_interrupt, (void*) &socket_cliente_interrupt);
		pthread_detach(hilo_interrupt);
        return 1;
    }
    return 0;
}




	

	
	
