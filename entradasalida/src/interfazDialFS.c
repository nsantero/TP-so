#include <interfazDialFS.h>


char* path_carpeta_FS = "/home/utnso/tp-2024-1c-File-System-Fanatics/entradasalida/FS/";

char* path_bitmap;


char* path_bloques;

// t configs son variables locales a DialFS, si es unico no necesita semaforo, 1 solo hilo

Interfaz interfaz_DialFS;
t_list * cola_procesos_DialFS=NULL;
pthread_mutex_t mutex_cola_DialFS = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_hay_en_DialFS;

void inicializar_sem_cola_DialFS(){
    sem_init(&sem_hay_en_DialFS,0,0);
}

void* manejo_interfaz_DialFS(){
    Peticion_Interfaz_DialFS* peticion_DialFS;

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
    
    inicializar_bloques_dat(aDevolver);
    inicializar_bitmap_dat(aDevolver);

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
    // internamente cada caso, po r q creo q hay un par q toca un registro
}

void inicializar_bloques_dat(Interfaz interfaz){

    
    path_bloques =generarPathAArchivoFS("bloques.dat");
    
    remove(path_bloques);
    
    FILE* bloquesDat= txt_open_for_append(path_bloques);

    size_t size =interfaz.blockCount*interfaz.blockSize;
    char buffer[size];
    memset(buffer,0,size);
    fwrite(buffer,1,size,bloquesDat);
    fflush(bloquesDat);

    txt_close_file(bloquesDat);

    
    //TODO no se como hacer q tenga el tamaño q tiene q tener, si llenarlo de ceros o q
    //tmb puedo hacer con config un set bloque x o algo asi no se y q lo lleve el bitmap.dat

    



}
void inicializar_bitmap_dat(Interfaz interfaz){
    
    
    
    path_bitmap=generarPathAArchivoFS("bitmap.dat");
    
    remove(path_bitmap);
    
    FILE* bitmapDat= txt_open_for_append(path_bitmap);

    size_t size =interfaz.blockCount/8;
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

    char* path=generarPathAArchivoFS(nombre);

    t_config* archivo=config_create(path);
    off_t bloqueInicial = config_get_double_value(archivo,"BLOQUE_INICIAL");
    int tamanioEnbytes = config_get_int_value(archivo,"TAMANIO_ARCHIVO");
    config_destroy(archivo);

    int cantBloques=tamanioEnbytes/interfaz_DialFS.blockSize;

    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);

    for(bloqueInicial;bloqueInicial<cantBloques;bloqueInicial++){
        bitarray_clean_bit(bitmapAddr,bloqueInicial);
    }
    

    bitarray_destroy(bitmapAddr);


    munmap(addr,sb.st_size);
    close(fd);


    remove(path);
}




void truncarArchivo(char* nombreArchivo,uint8_t tamanio){

}
void escribirEnArchivo(char* nombreArchivo,uint32_t direcion,uint8_t tamanio,uint32_t punteroArchivo){

}
void leerDelArchivo(char *nombreArchivo,uint32_t direcion,uint8_t tamanio,uint32_t punteroArchivo){

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




    munmap(addr,sb.st_size);
    close(fd);



    return offset;
}


char* generarPathAArchivoFS(char* nombreArchivo){

    return string_from_format("%s%s",path_carpeta_FS,nombreArchivo);



}