#include <interfazDialFS.h>


char* path_bitmap;
char* path_bloques;// estas se usan solo en un hilo asi q no necesitan semaforos

Interfaz interfaz_DialFS;
t_list * cola_procesos_DialFS=NULL;
pthread_mutex_t mutex_cola_DialFS = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_hay_en_DialFS;

void inicializar_sem_cola_DialFS(){
    sem_init(&sem_hay_en_DialFS,0,0);
}

void* manejo_interfaz_DialFS(){
    Peticion_Interfaz_DialFS* peticion_DialFS;

    inicializar_bloques_dat();
    inicializar_bitmap_dat();

    while(1){
        sem_wait(&sem_hay_en_DialFS);
        pthread_mutex_lock(&mutex_cola_DialFS);
        peticion_DialFS = list_remove(cola_procesos_DialFS,0);
        pthread_mutex_unlock(&mutex_cola_DialFS);

        EJECUTAR_INTERFAZ_DialFS(peticion_DialFS);



    }




}

Interfaz generarNuevaInterfazDialFS(char* nombre,char* pathConfiguracion){

    t_config * configuracion;
    configuracion = config_create(pathConfiguracion);

    Interfaz aDevolver;

    inicializar_sem_cola_DialFS();


    aDevolver.nombre=nombre;
    aDevolver.tipoInterfaz=config_get_string_value(configuracion,"TIPO_INTERFAZ");
    aDevolver.tiempoUnidadTrabajo=config_get_int_value(configuracion,"TIEMPO_UNIDAD_TRABAJO");
    aDevolver.ipKernel=config_get_string_value(configuracion,"IP_KERNEL");
    aDevolver.puertoKernel=config_get_string_value(configuracion,"PUERTO_KERNEL");
    aDevolver.blockCount=config_get_int_value(configuracion,"BLOCK_COUNT");
    aDevolver.blockSize=config_get_int_value(configuracion,"BLOCK_SIZE");
    aDevolver.ipMemoria=config_get_string_value(configuracion,"IP_MEMORIA");
    aDevolver.puertoMemoria=config_get_string_value(configuracion,"PUERTO_MEMORIA");
    aDevolver.pathBaseDialfs=config_get_string_value(configuracion,"PATH_BASE_DIALFS");;
    aDevolver.retrasoCompactacion=config_get_int_value(configuracion,"RETRASO_COMPACTACION");
    
   

    return aDevolver;

}

void EJECUTAR_INTERFAZ_DialFS(Peticion_Interfaz_DialFS* peticion){

    switch (peticion->operacion)
    {
    case DFS_CREATE:
        crearNuevoFile(peticion->nombreArchivo);
        break;
    case DFS_DELETE:
        borrarFile(peticion->nombreArchivo);
        break;
    case DFS_TRUNCATE:
        truncarArchivo(peticion->nombreArchivo,peticion->tamanio);
        break;
    case DFS_WRITE:
        escribirEnArchivo(peticion->nombreArchivo,peticion->direcion,peticion->tamanio,peticion->punteroArchivo);
        break;
    case DFS_READ:
        leerDelArchivo(peticion->nombreArchivo,peticion->direcion,peticion->tamanio,peticion->punteroArchivo);
        break;
        
    default:
        break;
    }

    //TODO avisar a kernel, por ahi deberia hacerlo
    // internamente cada caso, por q creo q hay un par q toca un registro
}

void inicializar_bloques_dat(){
  
    path_bloques =generarPathAArchivoFS("bloques.dat");
    
    remove(path_bloques);
    
    FILE* bloquesDat= txt_open_for_append(path_bloques);

    size_t size =interfaz_DialFS.blockCount*interfaz_DialFS.blockSize;
    char buffer[size];
    memset(buffer,0,size);
    fwrite(buffer,1,size,bloquesDat);
    fflush(bloquesDat);

    txt_close_file(bloquesDat);
}
void inicializar_bitmap_dat(){
    
    path_bitmap=generarPathAArchivoFS("bitmap.dat");
    
    remove(path_bitmap);
    
    FILE* bitmapDat= txt_open_for_append(path_bitmap);

    size_t size =(interfaz_DialFS.blockCount/8)+1;
    char buffer[size];
    memset(buffer,0,size);
    fwrite(buffer,1,size,bitmapDat);
    fflush(bitmapDat);

    txt_close_file(bitmapDat);
}


void crearNuevoFile(char* nombre){
    
    char* path=generarPathAArchivoFS(nombre);
    FILE*archivoNuevo=txt_open_for_append(path);

    txt_close_file(archivoNuevo);

    t_config* archivo=config_create(path);

    off_t bloqueInicialOffs=buscarBloqueLibre();
    char bloqueInicialChar[20];
    snprintf(bloqueInicialChar,20,"%ld",(long)bloqueInicialOffs);
    config_set_value(archivo,"BLOQUE_INICIAL",bloqueInicialChar);
    config_set_value(archivo,"TAMANIO_ARCHIVO","0");

    config_save(archivo);
    config_destroy(archivo);

    //aca deberia poner el bloque q elije como ocupado en el bit map

    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);


    bitarray_set_bit(bitmapAddr,bloqueInicialOffs);

    bitarray_destroy(bitmapAddr);


    munmap(addr,sb.st_size);
    close(fd);
    free(path);
}
void borrarFile(char* nombre){
    
    off_t bloqueInicial;
    int tamanioEnbytes;
    char* path=obtenerInfoDeArchivo(nombre,&bloqueInicial,&tamanioEnbytes);


    int cantBloques=(tamanioEnbytes/interfaz_DialFS.blockSize)+1;

    for(bloqueInicial;bloqueInicial<(cantBloques+bloqueInicial);bloqueInicial++){
        liberarBloque(bloqueInicial);
    }
    
    remove(path);
}
void truncarArchivo(char* nombreArchivo,uint8_t tamanio){
    off_t bloqueInicial;
    int tamanioEnbytesActual;
    char* path=obtenerInfoDeArchivo(nombreArchivo,&bloqueInicial,&tamanioEnbytesActual);
    off_t cantBloquesNecesarios=(tamanio/8);
    off_t cantbloquesActuales=(tamanioEnbytesActual/8);
     //caso hay q achicar el archivo, se liberan los bloques
        
    if(cantbloquesActuales>cantBloquesNecesarios){ //si la cant de bloquesActuales es igual(no puede ser menor) no se debe liberar ningun bloque
        for(cantbloquesActuales;cantbloquesActuales>cantBloquesNecesarios;cantbloquesActuales--){
            liberarBloque(cantbloquesActuales);                
        }   
    }else if(cantbloquesActuales==cantBloquesNecesarios){}//caso, no se necesita achicar ni agrandar
    else{//caso se debe agrandar el archivo
        
        int bloquesNuevosNecesarios=cantBloquesNecesarios-cantbloquesActuales;


        if(!existenBloquesDisponibles(bloquesNuevosNecesarios)){//caso no queda lugar en el FS

            //TODO tirar el error de q no hay lugar para agrandar el archivo
            /**
             * 
             * 
             * 
             * 
             * 
            */
        }else if(hayLugarDespuesDelArchivo(bloquesNuevosNecesarios,bloqueInicial+cantbloquesActuales)){//caso, hay lugar inmediatamentemente despues para argadarlo
            for(int i=1;i<=bloquesNuevosNecesarios;i++){
                ocuparBloque(bloqueInicial+cantbloquesActuales+i);
            }
        }else{// caso, se debe reorganizar el FS para acomodar el archivo
            //TODO aca hay q hacer la funcion q reacomoda todo y deja este ultimo al final o no se como mierda se hace
            /**
             * compactar(); LA FUNCION ESTA ENCIMA DEBERIA TENER CUIDADO DE COMO DEJA AL ARCHIVO Q QUEREMOS ESCRIBIR
             * DEBERIA QUEDAR AL FINAL
             * 
             * 
            */
        }

    }


}
void escribirEnArchivo(char* nombreArchivo,uint32_t direcion,uint8_t tamanio,uint32_t punteroArchivo){

}
void leerDelArchivo(char *nombreArchivo,uint32_t direcion,uint8_t tamanio,uint32_t punteroArchivo){

}


int hayLugarDespuesDelArchivo(int cantBloques,off_t ultimoBloqueDelArchivo){

    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);

    

    for (int i=1;i<=cantBloques;i++){
        if (bitarray_test_bit(bitmapAddr,ultimoBloqueDelArchivo+i)){
            bitarray_destroy(bitmapAddr);
            munmap(addr,sb.st_size);
            close(fd);
            return 0;
            }
    }


    bitarray_destroy(bitmapAddr);
    munmap(addr,sb.st_size);
    close(fd);

    return 1;

}
int existenBloquesDisponibles(int bloquesNecesarios){

    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);

    off_t offset=0;
    int contador=0;

    for (offset;offset<(sb.st_size*8);offset++){
        if (!bitarray_test_bit(bitmapAddr,offset)){
            contador++;
            if(contador>=bloquesNecesarios){
                bitarray_destroy(bitmapAddr);
                munmap(addr,sb.st_size);
                close(fd);
                return 1;
            }
        }
    }

    bitarray_destroy(bitmapAddr);
    munmap(addr,sb.st_size);
    close(fd);

    return 0;



}
off_t buscarBloqueLibre(){
    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);

    off_t offset=0;

    for (offset;offset<(sb.st_size*8);offset++){
        if (!bitarray_test_bit(bitmapAddr,offset)){break;}
    }

    bitarray_destroy(bitmapAddr);


    munmap(addr,sb.st_size);
    close(fd);



    return offset;
}

char* obtenerInfoDeArchivo(char* nombreArchivo,off_t* offset,int* tamanioEnBytes){
    char* path= generarPathAArchivoFS(nombreArchivo);
    t_config* archivo=config_create(path);
    offset = config_get_long_value(archivo,"BLOQUE_INICIAL");
    tamanioEnBytes = config_get_int_value(archivo,"TAMANIO_ARCHIVO");
    config_destroy(archivo);
    return path;
}

void liberarBloque(off_t offset){
    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);

   
    bitarray_clean_bit(bitmapAddr,offset);
    
    

    bitarray_destroy(bitmapAddr);


    munmap(addr,sb.st_size);
    close(fd);
}
void ocuparBloque(off_t offset){
    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);

   
    bitarray_set_bit(bitmapAddr,offset);
    
    

    bitarray_destroy(bitmapAddr);


    munmap(addr,sb.st_size);
    close(fd);
}

char* generarPathAArchivoFS(char* nombreArchivo){

    return string_from_format("%s%s",interfaz_DialFS.pathBaseDialfs,nombreArchivo);



}