#include <stdlib.h>
#include <stdio.h>
#include <entradasalida.h>


void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion

	if (config == NULL) {
		perror("Archivo de configuracion no encontrado");
		exit(-1);
	}

	// Revisar esto -> config_valores.nombre_interfaz = config_get_string_value(config,"NOMBRE_INTERFAZ");
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

void EJECUTAR_INTERFAZ_GENERICA(){
	void* unidades_trabajo = recibir_buffer(sizeof(int), kernel_fd);
	t_paquete* paquete_sleep = crear_paquete(IO_GEN_SLEEP);
	paquete_sleep->buffer = unidades_trabajo;
	enviar_paquete(paquete_sleep, kernel_fd);//le mando a kernel el paquete con la instruccion IO_GEN_SLEEP con las unidades
	free(unidades_trabajo);
	free(paquete_sleep->buffer);
}

void EJECUTAR_INTERFAZ_STDIN(){
	char* texto = NULL;
	size_t texto_lenght;
	ssize_t read;
	t_list* paquete_kernel = recibir_paquete(kernel_fd);
	printf("Ingrese el texto deseado: ");
	read = getline(&texto,&texto_lenght, stdin);
	t_list* paquete_buffer = crear_paquete(IO_STDIN_READ);
	//t_buffer* buffer_entrada = crear_buffer_aislado(texto,read);

		if (buffer_entrada != NULL) {
        	printf("Buffer creado con éxito.\n");
        	printf("Tamaño del buffer: %d\n", buffer_entrada->size);
        	printf("Stream del buffer: %s\n", (char*)buffer_entrada->stream);
    			} else {
        			printf("Error al crear el buffer.\n");
    			}

	enviar_paquete(paquete_kernel, memoria_fd); //le mando a memoria el paquete con las direcciones fisica de memoria
	enviar_buffer(buffer_entrada, memoria_fd); //le mando a memoria el buffer con el texto ingresado
	free(buffer_entrada->stream);
	free(buffer_entrada);
    free(texto);
}

void EJECUTAR_INTERFAZ_STDOUT(){
}

void EJECUTAR_INTERFAZ_DialFS(){

}

int main(int argc, char* argv[]) {

    logger = log_create("../kernel.log", "ENTRADASALIDA", true, LOG_LEVEL_INFO);
	log_info(logger, "Se creo el log!");

    cargar_configuracion("/home/utnso/tp-2024-1c-File-System-Fanatics/entradasalida/entradasalida.config");

	// Conecto entradasalida con kernel y memoria
	kernel_fd = crear_conexion(logger,"KERNEL",config_valores.ip_kernel,config_valores.puerto_kernel);
	log_info(logger, "Me conecte a kernel");
    memoria_fd = crear_conexion(logger,"MEMORIA",config_valores.ip_memoria,config_valores.puerto_memoria);
	log_info(logger, "Me conecte a memoria");

	config_valores.esta_conectada = true;

	char* tipoInterfaz = config_valores.tipo_interfaz;

	if(tipoInterfaz == "GENERICA"){
		EJECUTAR_INTERFAZ_GENERICA();
	}
		else if (tipoInterfaz == "STDIN"){
			EJECUTAR_INTERFAZ_STDIN();
		}
			else if (tipoInterfaz == "STDOUT"){
				EJECUTAR_INTERFAZ_STDOUT();
				}
				else{//Es del tipo DialFS
					EJECUTAR_INTERFAZ_DialFS();
	}

	// envio mensajes
	//enviar_mensaje("soy e/s", memoria_fd);
    //enviar_mensaje("soy e/s", kernel_fd);

    return EXIT_SUCCESS;
}
