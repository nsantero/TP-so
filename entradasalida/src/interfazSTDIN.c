#include <interfazSTDIN.h>

Interfaz interfaz_STDIN;
t_list * cola_procesos_STDIN = NULL;
pthread_mutex_t mutex_cola_STDIN = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_hay_en_cola_STDIN;

void inicializar_sem_cola_STDIN()
{
    cola_procesos_STDIN=list_create();
    sem_init(&sem_hay_en_cola_STDIN, 0, 0);
}

void* manejo_interfaz_STDIN(){
    Peticion_Interfaz_STDIN* peticion_STDIN;

    while(1){
        sem_wait(&sem_hay_en_cola_STDIN);
        pthread_mutex_lock(&mutex_cola_STDIN);
        peticion_STDIN = list_remove(cola_procesos_STDIN,0);
        pthread_mutex_unlock(&mutex_cola_STDIN);

        EJECUTAR_INTERFAZ_STDIN(peticion_STDIN);
        free(peticion_STDIN->nombre_interfaz);
        free(peticion_STDIN);
    }

}

Interfaz generarNuevaInterfazSTDIN(char* nombre,t_config* configuracion){

   

    Interfaz aDevolver;
    
    inicializar_sem_cola_STDIN();

    aDevolver.nombre=nombre;
    aDevolver.tipoInterfaz=config_get_string_value(configuracion,"TIPO_INTERFAZ");
    aDevolver.tiempoUnidadTrabajo=-1;
    aDevolver.ipKernel=config_get_string_value(configuracion,"IP_KERNEL");
    aDevolver.puertoKernel=config_get_string_value(configuracion,"PUERTO_KERNEL");
    aDevolver.blockCount=-1;
    aDevolver.blockSize=-1;
    aDevolver.ipMemoria=config_get_string_value(configuracion,"IP_MEMORIA");
    aDevolver.puertoMemoria=config_get_string_value(configuracion,"PUERTO_MEMORIA");
    aDevolver.pathBaseDialfs=NULL;
    aDevolver.retrasoCompactacion=-1;
    

    return aDevolver;
}

char* leer_texto_ingresado(uint32_t tamanio,int PID) {
    char *texto = malloc(tamanio+1);
    printf("Ingrese el texto deseado para el proceso de PID=%d. Tamaño maximo %d: \n",PID,tamanio);
    fgets(texto, tamanio+1, stdin);
    texto[strcspn(texto, "\n")] = '\0';//Para eliminar el \n
    printf(texto);
    return texto;
}

void EJECUTAR_INTERFAZ_STDIN(Peticion_Interfaz_STDIN* peticion){
	char* texto_leido = NULL;

	
	t_paquete* paquete_entrada = crear_paquete(IO_MEM_STDIN_READ);


	texto_leido = leer_texto_ingresado(peticion->tamanio,peticion->PID);
    peticion->tamanio=strlen(texto_leido)+1;
	agregar_entero_a_paquete32(paquete_entrada,peticion->tamanio);
    agregar_entero_a_paquete32(paquete_entrada,peticion->direccion);
    agregar_string_a_paquete(paquete_entrada,texto_leido); 

	printf("se envio paquete a memoria");
	enviar_paquete(paquete_entrada, memoria_fd); //le mando a memoria el buffer con el texto ingresado
	
    
    
    free(paquete_entrada->buffer->stream);
	free(paquete_entrada->buffer);
	free(paquete_entrada);
    free(texto_leido);

    log_info(loggerIO,"PID: %d - Operacion: IO_STDIN_READ",peticion->PID);
    printf("se envio q termino a kernel");
    terminoEjecucionInterfaz(interfaz_STDIN.nombre,peticion->PID);
    
}



