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


void crearInterfaces(){

   generarNuevaInterfazGenerica("interfazGenerica"/*TODO, deshardcodear esto*/,"/home/utnso/tp-2024-1c-Aprobado/entradasalida/configGenerica.config");
   
}


char* leer_texto_ingresado() {
    char *texto = NULL;
    size_t lenght = 0;
    printf("Ingrese el texto deseado: ");
    getline(&texto, &lenght, stdin);
    texto[strcspn(texto, "\n")] = '\0';//Para eliminar el \n
    return texto;
}

uint32_t recibir_direccion_fisica(){

	unsigned long direccion_fisica;
    printf("Ingrese una direccion de memoria en hexadecimal (ej: 0x7fff5fbffabc): ");
    scanf("%lx", &direccion_fisica);

    // Convierto la dirección ingresada en un puntero a un entero
    uint32_t direccion_memoria = (uint32_t)direccion_fisica;
	return direccion_memoria;
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

	interfaz_generica = generarNuevaInterfazGenerica("Int1","PATH");//TODO PATH
	

	pthread_t hilo_interfaz_generica;
	pthread_create(&hilo_interfaz_generica,NULL,manejo_interfaz_generica,NULL);

	interfaz_STDIN = generarNuevaInterfazSTDIN("Int2","PATH");//TODO PATH

	pthread_t hilo_interfaz_STDIN;
	pthread_create(&hilo_interfaz_STDIN,NULL,manejo_interfaz_STDIN,NULL);

	interfaz_STDOUT = generarNuevaInterfazSTDOUT("Int3","PATH");//TODO PATH

	pthread_t hilo_interfaz_STDOUT;
	pthread_create(&hilo_interfaz_STDOUT,NULL,manejo_interfaz_STDOUT,NULL);





	pthread_join(hilo_interfaz_generica,NULL);
	pthread_join(hilo_interfaz_STDIN,NULL);
	pthread_join(hilo_interfaz_STDOUT,NULL);
	//TODO agregar otros hilos
	
	
	
	
	
	
	
	char* tipoInterfaz = config_valores.tipo_interfaz;

	if(tipoInterfaz == "GENERICA"){
		
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
