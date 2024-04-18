#include "../include/main.h"

int main(int argc, char* argv[]) {


    logger = log_create("../kernel.log", "KERNEL", 0, LOG_LEVEL_INFO);

	FILE * archivo_configuracion = fopen(argv[1], "r");


	log_info(logger, "No se pudo leer el archivo");

	if (!archivo_configuracion) {
		log_info(logger, "No se pudo leer el archivo");
		exit(-1);
	}

	cargar_configuracion(archivo_configuracion);

	int server_fd = iniciar_servidor(logger, "127.0.0.1", config_valores.puerto_escucha);  //cambiar eso hardcodeado

	int memoria_fd = crear_conexion(logger,config_valores.ip_memoria, config_valores.puerto_memoria);
    int cpu_fd = crear_conexion(logger,config_valores.ip_cpu, config_valores.puerto_cpu_dispatch);

    return 0;
}
