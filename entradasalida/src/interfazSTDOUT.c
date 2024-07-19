#include <interfazSTDOUT.h>

Interfaz interfaz_STDOUT;
t_list * cola_procesos_STDOUT = NULL;
pthread_mutex_t mutex_cola_STDOUT = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_hay_en_cola_STDOUT;

void inicializar_sem_cola_STDOUT()
{
    cola_procesos_STDOUT=list_create();
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
        free(peticion_STDOUT->nombre_interfaz);
        free(peticion_STDOUT);
    }

}

Interfaz generarNuevaInterfazSTDOUT(char* nombre,t_config* configuracion){

    

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
	
    t_paquete* paquete_direccion = crear_paquete(IO_MEM_STDOUT_WRITE);
    agregar_entero_a_paquete8(paquete_direccion,peticion->tamanio);
    agregar_entero_a_paquete32(paquete_direccion, peticion->direccion);
    enviar_paquete(paquete_direccion, memoria_fd);//Envio a memoria la direccion logica ingresada
    free(paquete_direccion->buffer);
    free(paquete_direccion);

    //TODO, hacer esto con un void* buffer; CREO Q ESTA HECHO LO DEL BUFFER, SI NO NO SE A Q ME REFERI ACA
    int bytes;
    void* buffer = recibir_buffer(&bytes,memoria_fd);//Recibo el contenido de la direccion por parte de memoria
    //esto lo recibe aca asi? 
    char* contenido_memoria = buffer;

    

    printf("El contenido encontrado en la direccion de memoria %u es: %s\n", peticion->direccion, contenido_memoria);
    

    
    free(contenido_memoria);
    free(buffer);

    log_info(loggerIO,"PID: %d - Operacion: IO_STDOUT_WRITE",peticion->PID);
    terminoEjecucionInterfaz(interfaz_STDOUT.nombre,peticion->PID);
}