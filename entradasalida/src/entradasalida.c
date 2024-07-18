#include <entradasalida.h>

t_config *configCargaInterfaz;

uint32_t recibir_direccion_fisica(){

	unsigned long direccion_fisica;
    printf("Ingrese una direccion de memoria en hexadecimal (ej: 0x7fff5fbffabc): ");
    scanf("%lx", &direccion_fisica);

    // Convierto la dirección ingresada en un puntero a un entero
    uint32_t direccion_memoria = (uint32_t)direccion_fisica;
	return direccion_memoria;
}
void handleSiginitIO();

int main(int argc, char* argv[]) {

	signal(SIGINT,handleSiginitIO);

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
	
	
	

	configCargaInterfaz=config_create(string_from_format("%s%s%s",pathADirectorio,argv[1],".config"));
	if(configCargaInterfaz==NULL){
		log_info(loggerIO,"No se encuentra el archivo de congiguracion, se finaliza el programa");
		cerrarLogger();
		return 0;
	}

	log_info(loggerIO,"Se continuan los logs en %s.log",argv[1]);
	cerrarLogger();
	inicializarLoggerDeInterfaz(argv[1]);
	
	
	char* ip_kernel;
    char* puerto_kernel;
    char* ip_memoria;
    char* puerto_memoria;
	
	

	//Conecto entradasalida con kernel y memoria
	ip_kernel=config_get_string_value(configCargaInterfaz,"IP_KERNEL");
	ip_memoria=config_get_string_value(configCargaInterfaz,"IP_MEMORIA");
	puerto_kernel=config_get_string_value(configCargaInterfaz,"PUERTO_KERNEL");
	puerto_memoria=config_get_string_value(configCargaInterfaz,"PUERTO_MEMORIA");
	kernel_fd = crear_conexion(loggerIO,"KERNEL",ip_kernel,puerto_kernel);
	log_info(loggerIO, "Me conecte a kernel");
	//TODO agregar if para q la generica no conecte.
    memoria_fd = crear_conexion(loggerIO,"MEMORIA",ip_memoria,puerto_memoria);
	log_info(loggerIO, "Me conecte a memoria");

	enviarNuevaInterfazAKernel(configCargaInterfaz,argv[1]);
	log_info(loggerIO, "Se conecto la interfaz con kernel");

	char* tipoChar=config_get_string_value(configCargaInterfaz,"TIPO_INTERFAZ");
	Tipos_Interfaz tipo=obtenerTipoConString(tipoChar);


	switch (tipo)
	{
	case T_GENERICA:

		interfaz_generica = generarNuevaInterfazGenerica(argv[1],configCargaInterfaz);
	
		pthread_t hilo_interfaz_generica;
		pthread_create(&hilo_interfaz_generica,NULL,manejo_interfaz_generica,NULL);
		pthread_detach(hilo_interfaz_generica);
		recibirPeticionDeIO_GEN();
		break;
	case T_STDIN:
		interfaz_STDIN = generarNuevaInterfazSTDIN(argv[1],configCargaInterfaz);

		pthread_t hilo_interfaz_STDIN;
		pthread_create(&hilo_interfaz_STDIN,NULL,manejo_interfaz_STDIN,NULL);
		pthread_detach(hilo_interfaz_STDIN);
		recibirPeticionDeIO_STDIN();
		break;
	case T_STDOUT:
		interfaz_STDOUT = generarNuevaInterfazSTDOUT(argv[1],configCargaInterfaz);

		pthread_t hilo_interfaz_STDOUT;
		pthread_create(&hilo_interfaz_STDOUT,NULL,manejo_interfaz_STDOUT,NULL);
		pthread_detach(hilo_interfaz_STDOUT);
		recibirPeticionDeIO_STDOUT();
		break;
	case T_DFS:
		interfaz_DialFS = generarNuevaInterfazDialFS(argv[1],configCargaInterfaz);
		pthread_t hilo_interfaz_DialFS;
		pthread_create(&hilo_interfaz_DialFS,NULL,manejo_interfaz_DialFS,NULL);
		pthread_detach(hilo_interfaz_DialFS);
		recibirPeticionDeIO_DialFS();
		break;
	default:
		break;
	}

    return EXIT_SUCCESS;
}

void handleSiginitIO(){


    //cerrar hilo?
    //free de todo lo q tenga q hacer free TODO
    //posiblemente con un switch del tipo de interfaz q este manejando




/*s
	list_destroy_and_destroy_elements(cola_procesos_ig,Peticion_Interfaz_Generica);
	list_destroy_and_destroy_elements(cola_procesos_STDIN,);
	list_destroy_and_destroy_elements(cola_procesos_STDOUT,);
	list_destroy_and_destroy_elements(cola_procesos_DialFS,);*/
	
	close(memoria_fd);
	close(kernel_fd);
    config_destroy(configCargaInterfaz);
    log_info(loggerIO,"Se desconecta la interfaz.");
    cerrarLogger();
    exit(0);
}