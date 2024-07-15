#include <entradasalida.h>

uint32_t recibir_direccion_fisica(){

	unsigned long direccion_fisica;
    printf("Ingrese una direccion de memoria en hexadecimal (ej: 0x7fff5fbffabc): ");
    scanf("%lx", &direccion_fisica);

    // Convierto la dirección ingresada en un puntero a un entero
    uint32_t direccion_memoria = (uint32_t)direccion_fisica;
	return direccion_memoria;
}

int main(int argc, char* argv[]) {

    inicializarLogger();
	log_info(loggerIO, "Se inicio el main de enrada y salida");

    if(argc==1){
		printf("Se inicio IO sin especificar ninguna interfaz");
		log_info(loggerIO, "Se inicio IO sin especificar ninguna interfaz");
		cerrarLogger();
		return 0;
	}
	if(argc>2){
		printf("Se introdujo mas de una interfaz, se conectara solo la primera");
		log_info(loggerIO, "Se introdujo mas de una interfaz, se conectara solo la primera");
	}
	
	
	t_config *configCargaInterfaz;

	configCargaInterfaz=config_create(string_from_format("%s%s%s",pathADirectorio,argv[1],".config"));
	if(configCargaInterfaz==NULL){
		log_info(loggerIO,"No se encuentra el archivo de congiguracion, se finaliza el programa");
		cerrarLogger();
		return 0;
	}

	log_info(loggerIO,"Se continuan los logs en %s.log",argv[1]);
	cerrarLogger();
	inicializarLoggerDeInterfaz(argv[1]);
	
	
	entradasalida_config config_valores;
	
	

	//Conecto entradasalida con kernel y memoria
	config_valores.ip_kernel=config_get_string_value(configCargaInterfaz,"IP_KERNEL");
	config_valores.ip_memoria=config_get_string_value(configCargaInterfaz,"IP_MEMORIA");
	config_valores.puerto_kernel=config_get_string_value(configCargaInterfaz,"PUERTO_KERNEL");
	config_valores.puerto_memoria=config_get_string_value(configCargaInterfaz,"PUERTO_MEMORIA");
	kernel_fd = crear_conexion(loggerIO,"KERNEL",config_valores.ip_kernel,config_valores.puerto_kernel);
	log_info(loggerIO, "Me conecte a kernel");
    memoria_fd = crear_conexion(loggerIO,"MEMORIA",config_valores.ip_memoria,config_valores.puerto_memoria);
	log_info(loggerIO, "Me conecte a memoria");

	enviarNuevaInterfazAKernel(configCargaInterfaz,argv[1]);
	log_info(loggerIO, "Se conecto la interfaz con kernel");

	char* tipoChar=config_get_string_value(configCargaInterfaz,"TIPO_INTERFAZ");
	Tipos_Interfaz tipo=obtenerTipoConString(tipoChar);


	switch (tipo)
	{
	case T_GENERICA:
		interfaz_generica = generarNuevaInterfazGenerica(argv[1],configCargaInterfaz);//TODO PATH
	
		pthread_t hilo_interfaz_generica;
		pthread_create(&hilo_interfaz_generica,NULL,manejo_interfaz_generica,NULL);
		pthread_join(hilo_interfaz_generica,NULL);
		recibirPeticionDeIO_GEN();
		break;
	case T_STDIN:
		interfaz_STDIN = generarNuevaInterfazSTDIN(argv[1],configCargaInterfaz);//TODO PATH

		pthread_t hilo_interfaz_STDIN;
		pthread_create(&hilo_interfaz_STDIN,NULL,manejo_interfaz_STDIN,NULL);
		pthread_join(hilo_interfaz_STDIN,NULL);
		recibirPeticionDeIO_STDIN();
		break;
	case T_STDOUT:
		interfaz_STDOUT = generarNuevaInterfazSTDOUT(argv[1],configCargaInterfaz);//TODO PATH

		pthread_t hilo_interfaz_STDOUT;
		pthread_create(&hilo_interfaz_STDOUT,NULL,manejo_interfaz_STDOUT,NULL);
		pthread_join(hilo_interfaz_STDOUT,NULL);
		recibirPeticionDeIO_STDOUT();
		break;
	case T_DFS:
		interfaz_DialFS = generarNuevaInterfazDialFS(argv[1],configCargaInterfaz);//TODO path

		pthread_t hilo_interfaz_DialFS;
		pthread_create(&hilo_interfaz_DialFS,NULL,manejo_interfaz_DialFS,NULL);
		pthread_join(hilo_interfaz_DialFS,NULL);
		recibirPeticionDeIO_DialFS();
		break;
	default:
		break;
	}

	





	
	
	


	// envio mensajes
	//enviar_mensaje("soy e/s", memoria_fd);
    //enviar_mensaje("soy e/s", kernel_fd);

    return EXIT_SUCCESS;
}
