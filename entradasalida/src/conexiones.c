#include <conexiones.h>
#include <interfazDialFS.h>
#include <interfazGenerica.h>
#include <interfazSTDIN.h>
#include <interfazSTDOUT.h>


int kernel_fd=0;
int memoria_fd=0;

void enviarNuevaInterfazAKernel(t_config* configNueva,char* nombre){

    int tamanioNombre=strlen(nombre)+1;//+1
    Tipos_Interfaz tipoDeLaInterfaz=obtenerTipoConString(config_get_string_value(configNueva,"TIPO_INTERFAZ"));

    t_paquete* paquete=crear_paquete(AGREGAR_INTERFACES);
    agregar_entero_a_paquete32(paquete,tamanioNombre);
    agregar_string_a_paquete(paquete,nombre);
    agregar_a_paquete(paquete,&tipoDeLaInterfaz,sizeof(Tipos_Interfaz));
    enviar_paquete(paquete,kernel_fd);
    eliminar_paquete(paquete);

}

void recibirPeticionDeIO_GEN(){
    
    while(1){
    
        t_paquete* paquete = NULL;
        paquete = malloc(sizeof(t_paquete));
        paquete->buffer = NULL;
        paquete->buffer = malloc(sizeof(t_buffer));
        paquete->buffer->stream = NULL;

        recv(kernel_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(kernel_fd, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(kernel_fd, paquete->buffer->stream, paquete->buffer->size, 0);
        void *stream = paquete->buffer->stream;

        switch (paquete->codigo_operacion)  
        {
            case IO_GEN_SLEEP:
            {
                Peticion_Interfaz_Generica *peticion=malloc(sizeof(Peticion_Interfaz_Generica));
                int bytes;
                
                stream+=sizeof(int);
                memcpy(&peticion->unidades_de_trabajo, stream, sizeof(int));
                stream += sizeof(int);
                stream += sizeof(int);
                memcpy(&peticion->PID, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&bytes, stream, sizeof(int));
                stream += sizeof(int);
                peticion->nombre_interfaz = malloc(bytes);
                memcpy(peticion->nombre_interfaz, stream, bytes);

            

                pthread_mutex_lock(&mutex_cola_ig);
                list_add(cola_procesos_ig,peticion);
                pthread_mutex_unlock(&mutex_cola_ig);
                sem_post(&sem_hay_en_cola_ig);

                

                break;
            }
            default:
            {
                //log_info(loggerIO,"Llega peticion incompatible");
                break;
            }
           
        }
        free(paquete->buffer->stream);
        free(paquete->buffer);
        free(paquete);

    }
}
void recibirPeticionDeIO_STDIN(){
    while(1){
    
        
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = NULL;
        paquete->buffer = malloc(sizeof(t_buffer));
        paquete->codigo_operacion = 0;
        paquete->buffer->size = 0;
        paquete->buffer->stream = NULL;

        recv(kernel_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(kernel_fd, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(kernel_fd, paquete->buffer->stream, paquete->buffer->size, 0);
        void *stream = paquete->buffer->stream;

        switch (paquete->codigo_operacion)  
        {
            case IO_STDIN_READ:
            {
                Peticion_Interfaz_STDIN *peticion=malloc(sizeof(Peticion_Interfaz_STDIN));
                peticion->frames=list_create();
                int bytes;
                int cantPags;
                
                memcpy(&peticion->direccion_fisica.bytes, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->direccion_fisica.desplazamiento, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->direccion_fisica.numero_frame, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->tamanio, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->PID, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&bytes, stream, sizeof(int));
                stream+=sizeof(int);
                peticion->nombre_interfaz = malloc(bytes);
                memcpy(peticion->nombre_interfaz, stream, bytes);
                stream+=bytes;
                memcpy(&peticion->tamPag, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&cantPags, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);

                for(int i =1; i<cantPags; i++){
                    uint32_t *frameAux=malloc(sizeof(uint32_t));
                    memcpy(frameAux, stream, sizeof(uint32_t));
                    stream+=sizeof(uint32_t);
                    list_add(peticion->frames, frameAux);
                    //free(frameAux);
				}		


                pthread_mutex_lock(&mutex_cola_STDIN);
                list_add(cola_procesos_STDIN,peticion);
                pthread_mutex_unlock(&mutex_cola_STDIN);
                sem_post(&sem_hay_en_cola_STDIN);

                break;

            }   
            default:
            {
                //log_info(loggerIO,"Llega peticion incompatible");
                break;
            }  
        }
        free(paquete->buffer->stream);
        free(paquete->buffer);
        free(paquete);

    }

}
void recibirPeticionDeIO_STDOUT(){
    while(1){
    
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = NULL;
        paquete->buffer = malloc(sizeof(t_buffer));
        paquete->codigo_operacion = 0;
        paquete->buffer->size = 0;
        paquete->buffer->stream = NULL;

        recv(kernel_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(kernel_fd, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(kernel_fd, paquete->buffer->stream, paquete->buffer->size, 0);
        void *stream = paquete->buffer->stream;

        switch (paquete->codigo_operacion)  
        {
            case IO_STDOUT_WRITE:
            {
                Peticion_Interfaz_STDOUT *peticion=malloc(sizeof(Peticion_Interfaz_STDOUT));
                
                peticion->frames=list_create();
                int bytes;
                int cantPags;
                
                memcpy(&peticion->direccion_fisica.bytes, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->direccion_fisica.desplazamiento, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->direccion_fisica.numero_frame, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->tamanio, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->PID, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&bytes, stream, sizeof(int));
                stream+=sizeof(int);
                peticion->nombre_interfaz = malloc(bytes);
                memcpy(peticion->nombre_interfaz, stream, bytes);
                stream+=bytes;
                memcpy(&peticion->tamPag, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&cantPags, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);

                for(int i =1; i<cantPags; i++){
                    uint32_t *frameAux=malloc(sizeof(uint32_t));
                    memcpy(frameAux, stream, sizeof(uint32_t));
                    stream+=sizeof(uint32_t);
                    list_add(peticion->frames, frameAux);
				}	

                pthread_mutex_lock(&mutex_cola_STDOUT);
                list_add(cola_procesos_STDOUT,peticion);
                pthread_mutex_unlock(&mutex_cola_STDOUT);
                sem_post(&sem_hay_en_cola_STDOUT);

                break;
                
            }
            default:
            {
                //log_info(loggerIO,"Llega peticion incompatible");
                break;
            }
        }
        free(paquete->buffer->stream);
        free(paquete->buffer);
        free(paquete);


    }

}
void recibirPeticionDeIO_DialFS(){
    while(1){
    
        
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = NULL;
        paquete->buffer = malloc(sizeof(t_buffer));
        paquete->codigo_operacion = 0;
        paquete->buffer->size = 0;
        paquete->buffer->stream = NULL;

        recv(kernel_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(kernel_fd, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(kernel_fd, paquete->buffer->stream, paquete->buffer->size, 0);
        void *stream = paquete->buffer->stream;

        
        int bytes;
        int cantPags;

        switch (paquete->codigo_operacion)  
        {
            case IO_FS_CREATE:
            case IO_FS_DELETE:
            {
                Peticion_Interfaz_DialFS *peticion=malloc(sizeof(Peticion_Interfaz_DialFS));
                peticion->frames=list_create();
                stream+=sizeof(int);
                memcpy(&peticion->operacion, stream, sizeof(OperacionesDeDialFS));
                stream += sizeof(OperacionesDeDialFS);
                memcpy(&bytes, stream, sizeof(int));
                stream += sizeof(int);
                peticion->nombreArchivo = malloc(bytes);
                memcpy(peticion->nombreArchivo, stream, bytes);
                stream+=bytes;
                memcpy(&peticion->PID, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&bytes, stream, sizeof(int));
                stream += sizeof(int);
                peticion->nombre_interfaz = malloc(bytes);
                memcpy(peticion->nombre_interfaz, stream, bytes);

                pthread_mutex_lock(&mutex_cola_DialFS);
                list_add(cola_procesos_DialFS,peticion);
                pthread_mutex_unlock(&mutex_cola_DialFS);
                sem_post(&sem_hay_en_DialFS);
                break;
            }
            case IO_FS_TRUNCATE:
            {
                Peticion_Interfaz_DialFS *peticion=malloc(sizeof(Peticion_Interfaz_DialFS));
                peticion->frames=list_create();
                stream+=sizeof(int);
                memcpy(&peticion->operacion, stream, sizeof(OperacionesDeDialFS));
                stream += sizeof(OperacionesDeDialFS);
                memcpy(&bytes, stream, sizeof(int));
                stream += sizeof(int);
                peticion->nombreArchivo = malloc(bytes);
                memcpy(peticion->nombreArchivo, stream, bytes);
                stream+=bytes;
                memcpy(&peticion->tamanio, stream, sizeof(uint32_t));//TODO, aca necesito q sea uint32.
                stream += sizeof(uint32_t);
                memcpy(&peticion->PID, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&bytes, stream, sizeof(int));
                stream += sizeof(int);
                peticion->nombre_interfaz = malloc(bytes);
                memcpy(peticion->nombre_interfaz, stream, bytes);

                pthread_mutex_lock(&mutex_cola_DialFS);
                list_add(cola_procesos_DialFS,peticion);
                pthread_mutex_unlock(&mutex_cola_DialFS);
                sem_post(&sem_hay_en_DialFS);
                break;
            }
            case IO_FS_READ:
            case IO_FS_WRITE:
            {
            //TODO separar los casos +- esta
                Peticion_Interfaz_DialFS *peticion=malloc(sizeof(Peticion_Interfaz_DialFS));
                peticion->frames=list_create();
                stream+=sizeof(int);
                memcpy(&peticion->operacion, stream, sizeof(OperacionesDeDialFS));
                stream += sizeof(OperacionesDeDialFS);
                memcpy(&bytes, stream, sizeof(int));
                stream += sizeof(int);
                peticion->nombreArchivo = malloc(bytes);
                memcpy(peticion->nombreArchivo, stream, bytes);
                stream+=bytes;
                memcpy(&peticion->Direccion_fisica.bytes, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->Direccion_fisica.desplazamiento, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->Direccion_fisica.numero_frame, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->tamanio, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->punteroArchivo, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&peticion->PID, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&bytes, stream, sizeof(int));
                stream += sizeof(int);
                peticion->nombre_interfaz = malloc(bytes);
                memcpy(peticion->nombre_interfaz, stream, bytes);
                stream+=bytes;
                memcpy(&peticion->tamPag, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
                memcpy(&cantPags, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);

                for(int i =1; i<cantPags; i++){
                    uint32_t *frameAux=malloc(sizeof(uint32_t));
                    memcpy(frameAux, stream, sizeof(uint32_t));
                    stream+=sizeof(uint32_t);
                    list_add(peticion->frames, frameAux);
				}	

                pthread_mutex_lock(&mutex_cola_DialFS);
                list_add(cola_procesos_DialFS,peticion);
                pthread_mutex_unlock(&mutex_cola_DialFS);
                sem_post(&sem_hay_en_DialFS);

                break;
            }
            default:
                //log_info(loggerIO,"Llega peticion incompatible");
                break;
            }
        free(paquete->buffer->stream);
        free(paquete->buffer);
        free(paquete);


    }

}

void avisarErrorAKernel(char* nombre,int PID){
    int tamanioNombre=string_length(nombre);
    

    t_paquete* paquete=crear_paquete(ERROR_EN_INTERFAZ);
    agregar_a_paquete(paquete,nombre,tamanioNombre);
    agregar_entero_a_paquete32(paquete,PID);
    enviar_paquete(paquete,kernel_fd);
    eliminar_paquete(paquete);
}
void terminoEjecucionInterfaz(char* nombre,int PID){
    int tamanioNombre=string_length(nombre);
    

    t_paquete* paquete=crear_paquete(DESBLOQUEAR_PROCESO_POR_IO);
    agregar_a_paquete(paquete,nombre,tamanioNombre);
    agregar_entero_a_paquete32(paquete,PID);
    enviar_paquete(paquete,kernel_fd);
    eliminar_paquete(paquete);
}
int enviarFragmentoAMemoria(op_code operacion, int pid, int marco, int desplazamiento,int size, void* datos){
    int devolver=0;
    t_paquete *paquete_memoria = crear_paquete(operacion);

    agregar_entero_a_paquete32(paquete_memoria, pid);
    agregar_entero_a_paquete32(paquete_memoria, marco);
    agregar_entero_a_paquete32(paquete_memoria, desplazamiento);
    agregar_a_paquete(paquete_memoria,datos, size);
    
    enviar_paquete(paquete_memoria, memoria_fd);
    free(paquete_memoria->buffer->stream);
    free(paquete_memoria->buffer);
    free(paquete_memoria);
    
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    recv(memoria_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
    recv(memoria_fd, &(paquete->buffer->size), sizeof(int), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(memoria_fd, paquete->buffer->stream, paquete->buffer->size, 0);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    if(paquete->codigo_operacion==OK){
        devolver=1;
    }
    else{
        devolver=-1;
    }
    
    free(paquete);
    return devolver;

}
void* solicitarFragmentoAMemoria(op_code operacion, int pid, int marco, int desplazamiento,int size){
    t_paquete *paquete_memoria = crear_paquete(operacion);

    agregar_entero_a_paquete32(paquete_memoria, pid);
    agregar_entero_a_paquete32(paquete_memoria, marco);
    agregar_entero_a_paquete32(paquete_memoria, desplazamiento);
    agregar_entero_a_paquete32(paquete_memoria, size);
    
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);

    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    recv(memoria_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
    recv(memoria_fd, &(paquete->buffer->size), sizeof(int), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(memoria_fd, paquete->buffer->stream, paquete->buffer->size, 0);
    void *stream = paquete->buffer->stream;
    void* buffer=malloc(size);
    memcpy(buffer,stream+4,size);
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
    return buffer;


}



/*
Peticion_Interfaz_STDIN* inicializarPeticionSTDIN(){
    Peticion_Interfaz_STDIN* peticion;
    peticion=malloc
}*/