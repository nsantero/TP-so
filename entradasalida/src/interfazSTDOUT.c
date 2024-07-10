#include <interfazSTDOUT.h>

Interfaz interfaz_STDOUT;
t_list * cola_procesos_STDOUT = NULL;
pthread_mutex_t mutex_cola_STDOUT = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_hay_en_cola_STDOUT;

void inicializar_sem_cola_STDOUT()
{
    sem_init(&sem_hay_en_cola_STDOUT, 0, 0);
}

void* manejo_interfaz_STDOUT(){
    Peticion_Interfaz_STDOUT* peticion_STDOUT;

    while(1){
        sem_wait(&sem_hay_en_cola_STDOUT);
        pthread_mutex_lock(&mutex_cola_STDOUT);
        peticion_STDOUT = list_remove(cola_procesos_STDOUT,0);
        pthread_mutex_unlock(&mutex_cola_STDOUT);

        EJECUTAR_INTERFAZ_STDOUT(peticion_STDOUT);

    }

}

Interfaz generarNuevaInterfazSTDOUT(char* nombre,char* pathConfiguracion){

    t_config * configuracion;
    configuracion = config_create(pathConfiguracion);

    Interfaz aDevolver;
    
    inicializar_sem_cola_STDOUT();

    aDevolver.nombre=nombre;
    aDevolver.tipoInterfaz=config_get_string_value(configuracion,"TIPO_INTERFAZ");
    aDevolver.tiempoUnidadTrabajo=NULL;
    aDevolver.ipKernel=config_get_string_value(configuracion,"IP_KERNEL");
    aDevolver.puertoKernel=config_get_string_value(configuracion,"PUERTO_KERNEL");
    aDevolver.blockCount=NULL;
    aDevolver.blockSize=NULL;
    aDevolver.ipMemoria=config_get_string_value(configuracion,"IP_MEMORIA");
    aDevolver.puertoMemoria=config_get_string_value(configuracion,"PUERTO_MEMORIA");
    aDevolver.pathBaseDialfs=NULL;
    aDevolver.retrasoCompactacion=NULL;
    

    return aDevolver;
}

void EJECUTAR_INTERFAZ_STDOUT(Peticion_Interfaz_STDOUT* peticion){
	
    uint32_t direccion_ingresada = recibir_direccion_fisica();
    t_paquete* paquete_direccion = crear_paquete(IO_STDOUT_WRITE);
    agregar_entero_a_paquete(paquete_direccion, direccion_ingresada);

    enviar_paquete(paquete_direccion, memoria_fd);//Envio a memoria la direccion fisica ingresada

    t_list* paquete_memoria = recibir_paquete(memoria_fd);//Recibo el contenido de la direccion por parte de memoria

    char* contenido_memoria = string_new();

    contenido_memoria = paquete_memoria->head;

    printf("El contenido encontrado en la direccion de memoria %u es %s", direccion_ingresada, contenido_memoria);

    free(paquete_direccion->buffer);
    free(paquete_direccion);
    free(paquete_memoria);
    free(contenido_memoria);


    log_info(loggerIO,"PID: %d - Operacion: IO_STDOUT_WRITE",peticion->PID);
}