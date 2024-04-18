#include "../include/main.h"

int main(int argc, char* argv[]) {
    
    //decir_hola("CPU");

    logger = log_create("../cpu.log", "CPU", 0, LOG_LEVEL_INFO);

	FILE * archivo_configuracion = fopen(argv[1], "r");

	if (!archivo_configuracion) {
		log_info(logger, "No se pudo leer el archivo");
		exit(-1);
	}

	cargar_configuracion(archivo_configuracion);

	int memoria_fd = crear_conexion(logger,config_valores.ip_memoria, config_valores.puerto_memoria);
	int server_dispatch_fd = iniciar_servidor(logger,  "127.0.0.1", config_valores.puerto_escucha_dispatch);  //cambiar eso hardcodeado

//	while(server_escuchar(server_dispatch_fd));

    return 0;
}
