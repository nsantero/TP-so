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
        list_destroy(peticion_STDOUT->frames);
        free(peticion_STDOUT->nombre_interfaz);
        free(peticion_STDOUT);
    }

}

Interfaz generarNuevaInterfazSTDOUT(char* nombre,t_config* configuracion){

    

    Interfaz aDevolver;
    
    inicializar_sem_cola_STDOUT();

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

void EJECUTAR_INTERFAZ_STDOUT(Peticion_Interfaz_STDOUT* peticion){
	
    

	uint32_t* marco=NULL;
	char* leidoEnMemoria=NULL;
    leidoEnMemoria=malloc(peticion->tamanio+1);
    *leidoEnMemoria='2';
    uint32_t bytes=peticion->direccion_fisica.bytes;
	
    
	void* buffer=NULL;
    buffer=solicitarFragmentoAMemoria(IO_MEM_STDOUT_WRITE,peticion->PID,peticion->direccion_fisica.numero_frame,peticion->direccion_fisica.desplazamiento,bytes);
    memcpy(leidoEnMemoria,buffer,bytes);

    free(buffer);
    while(!list_is_empty(peticion->frames)){
        

        if (list_size(peticion->frames)>1){
            
            marco=list_remove(peticion->frames,0);
            buffer=solicitarFragmentoAMemoria(IO_MEM_STDOUT_WRITE,peticion->PID,*marco,0,peticion->tamPag);
            memcpy(leidoEnMemoria+bytes,buffer,peticion->tamPag);
            bytes+=peticion->tamPag;
            
            free(marco);
            free(buffer);
        }else{
            
            marco=list_remove(peticion->frames,0);
            buffer=solicitarFragmentoAMemoria(IO_MEM_STDOUT_WRITE,peticion->PID,*marco,0,peticion->tamanio-bytes);
            memcpy(leidoEnMemoria+bytes,buffer,peticion->tamanio-bytes);

            free(marco);
            free(buffer);
        }

    }

    

    
    leidoEnMemoria[peticion->tamanio] = '\0';
    printf("El contenido encontrado en memoria es: %s\n", leidoEnMemoria);
    fflush(stdout);
    
    free(leidoEnMemoria);
    

    log_info(loggerIO,"PID: %d - Operacion: IO_STDOUT_WRITE",peticion->PID);
    terminoEjecucionInterfaz(interfaz_STDOUT.nombre,peticion->PID);
}