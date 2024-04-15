#include ".../include/main.h"

int main(int argc, char* argv[]) {


    logger = log_create("../memoria.log", "MEMORIA", 0, LOG_LEVEL_INFO);

	FILE * archivo_configuracion = fopen(argv[1], "r");

	if (!archivo_configuracion) {
		log_info(logger, "No se pudo leer el archivo");
		exit(-1);
	}

	cargar_configuracion(archivo_configuracion);

	int server_fd = iniciar_servidor(logger, "127.0.0.1", config_valores.puerto_escucha;  //cambiar eso hardcodeado

	while(server_escuchar(server_fd));

    return 0;
}
