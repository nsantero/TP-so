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
        list_destroy(peticion_STDIN->frames);
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
    char* textoLeido=NULL;
    char* textoAdevolver=NULL;
    printf("Ingrese el texto deseado para el proceso de PID=%d. Tamaño maximo %d: \n",PID,tamanio);
    fflush(stdout);
    textoLeido=readline("> ");
    
    if (strlen(textoLeido)>=tamanio){
        textoAdevolver=textoLeido;
    }else{
        textoAdevolver=malloc(tamanio+1);
        textoAdevolver[tamanio]='\0';
        memcpy(textoAdevolver,textoLeido,tamanio);
        free(textoLeido);
    }
        
    
    
    return textoAdevolver;
}




void EJECUTAR_INTERFAZ_STDIN(Peticion_Interfaz_STDIN* peticion){
	char* texto_leido = NULL;

	uint32_t* marco=NULL;
	

    uint32_t bytes=peticion->direccion_fisica.bytes;
	texto_leido = leer_texto_ingresado(peticion->tamanio,peticion->PID);
    //peticion->tamanio=strlen(texto_leido)+1;
	void* buffer=NULL;
    
    buffer=malloc(bytes);
    memcpy(buffer,texto_leido,peticion->direccion_fisica.bytes);
    enviarFragmentoAMemoria(IO_MEM_STDIN_READ,peticion->PID,peticion->direccion_fisica.numero_frame,peticion->direccion_fisica.desplazamiento,bytes,buffer);
    
    free(buffer);

    while(!list_is_empty(peticion->frames)){
        

        if(list_size(peticion->frames)>1){
            buffer=malloc(peticion->tamPag);
            memcpy(buffer,texto_leido+bytes,peticion->tamPag);
            marco=list_remove(peticion->frames,0);
            enviarFragmentoAMemoria(IO_MEM_STDIN_READ,peticion->PID,*marco,0,peticion->tamPag,buffer);
            
            bytes+=peticion->tamPag;
            
            free(marco);
            free(buffer);
        }else{
            buffer=malloc(peticion->tamanio-bytes);
            memcpy(buffer,texto_leido+bytes,peticion->tamanio-bytes);
            marco=list_remove(peticion->frames,0);
            enviarFragmentoAMemoria(IO_MEM_STDIN_READ,peticion->PID,*marco,0,peticion->tamanio-bytes,buffer);
            
            free(marco);
            free(buffer);
        }

    }
    
	
	
    
    
    
    free(texto_leido);

    log_info(loggerIO,"PID: %d - Operacion: IO_STDIN_READ",peticion->PID);
    
    terminoEjecucionInterfaz(interfaz_STDIN.nombre,peticion->PID);
    
}



