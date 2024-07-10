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
    
    DIR *dir=opendir(aDevolver.pathBaseDialfs);

    int bloquesCheckeo=1;
    int bitCheckeo=1;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if(entry->d_name=="bloques.dat"){
            bloquesCheckeo=0;
        }else if(entry->d_name=="bitmap.dat"){
            bitCheckeo=0;
        }


    }

    close(dir);
    if(bloquesCheckeo){inicializar_bloques_dat(aDevolver);}
    if(bitCheckeo){inicializar_bitmap_dat(aDevolver);}

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

void inicializar_bloques_dat(Interfaz interfaz){
  
    path_bloques =string_from_format("%s%s",interfaz.pathBaseDialfs,"bloques.dat");
    
    FILE* bloquesDat= txt_open_for_append(path_bloques);

    size_t size =interfaz.blockCount*interfaz.blockSize;
    char buffer[size];
    memset(buffer,0,size);
    fwrite(buffer,1,size,bloquesDat);
    fflush(bloquesDat);

    txt_close_file(bloquesDat);
}
void inicializar_bitmap_dat(Interfaz interfaz){
    
    path_bitmap=string_from_format("%s%s",interfaz.pathBaseDialfs,"bitmap.dat");
    
    FILE* bitmapDat= txt_open_for_append(path_bitmap);

    size_t size =(interfaz.blockCount/8)+1;
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
        cambiarInfoDeArchivo(nombreArchivo,NULL,tamanio);  


    }else if(cantbloquesActuales==cantBloquesNecesarios){
        cambiarInfoDeArchivo(nombreArchivo,NULL,tamanio);
    }//caso, no se necesita achicar ni agrandar
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
            cambiarInfoDeArchivo(nombreArchivo,NULL,tamanio);  



        }else{// caso, se debe reorganizar el FS para acomodar el archivo
            //TODO aca hay q hacer la funcion q reacomoda todo y deja este ultimo al final o no se como mierda se hace
            /**
             * compactar(); LA FUNCION ESTA ENCIMA DEBERIA TENER CUIDADO DE COMO DEJA AL ARCHIVO Q QUEREMOS ESCRIBIR
             * DEBERIA QUEDAR AL FINAL
             * 
             * 
            */
            compactarBloquesFSParaQEntreElArchivo(nombreArchivo,&bloqueInicial,tamanioEnbytesActual);

        }

    }


}
void escribirEnArchivo(char* nombreArchivo,uint32_t direcion,uint8_t tamanio,uint32_t punteroArchivo){

}
void leerDelArchivo(char *nombreArchivo,uint32_t direcion,uint8_t tamanio,uint32_t punteroArchivo){

}

//TODO estas dos no tienen en cuenta q el ultimo byte del bitmap puede q tenga hasta 7 bits de mas, buscar bloque libre tmp
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

/**
 * busca el primer bloque vacio en el FS
 * devuelve -1 si no hay lugar
*/

off_t buscarBloqueLibre(){
    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);
    int encontrado=0;
    off_t offset=0;

    for (offset;offset<(sb.st_size*8);offset++){
        if (!bitarray_test_bit(bitmapAddr,offset)){encontrado=1;break;}
    }

    bitarray_destroy(bitmapAddr);


    munmap(addr,sb.st_size);
    close(fd);

    if(encontrado){
        return offset;
    }else{
        return -1;
    }
}
off_t buscarBloqueLibreDesdeElFinal(){
    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);
    int encontrado=0;
    off_t offset=interfaz_DialFS.blockCount-1;

    for (offset;offset>=0;offset--){
        if (!bitarray_test_bit(bitmapAddr,offset)){encontrado=1;break;}
    }

    bitarray_destroy(bitmapAddr);


    munmap(addr,sb.st_size);
    close(fd);


    if(encontrado){
        return offset;
    }else{
        return -1;
    }


}

char* obtenerInfoDeArchivo(char* nombreArchivo,off_t* offset,int* tamanioEnBytes){
    char* path= generarPathAArchivoFS(nombreArchivo);
    t_config* archivo=config_create(path);
    if(offset!=NULL){obtenerInfoDeArchivoOffset(nombreArchivo,offset);}
    if(tamanioEnBytes!=NULL){obtenerInfoDeArchivoTamanio(nombreArchivo,tamanioEnBytes);}
    config_destroy(archivo);
    return path;
}
void obtenerInfoDeArchivoOffset(char* nombreArchivo,off_t* offset){
    char* path= generarPathAArchivoFS(nombreArchivo);
    t_config* archivo=config_create(path);
    offset = config_get_long_value(archivo,"BLOQUE_INICIAL");
    config_destroy(archivo);
    return path;
}
void obtenerInfoDeArchivoTamanio(char* nombreArchivo,int* tamanioEnBytes){
    char* path= generarPathAArchivoFS(nombreArchivo);
    t_config* archivo=config_create(path);
    tamanioEnBytes = config_get_int_value(archivo,"TAMANIO_ARCHIVO");
    config_destroy(archivo);
    return path;
}
void cambiarInfoDeArchivo(char* nombreArchivo,off_t offset,int tamanioEnBytes){
    char* path= generarPathAArchivoFS(nombreArchivo);
    t_config* archivo=config_create(path);
    if(offset!=NULL){
        char offsetChar[20];
        snprintf(offsetChar,20,"%ld",(long)offset);
        config_set_value(archivo,"BLOQUE_INICIAL",offsetChar);
    }
    if(tamanioEnBytes!=NULL){
        char* tamanioChar=string_from_format("%d",tamanioEnBytes);
        config_set_value(archivo,"TAMANIO_ARCHIVO",tamanioChar);
    }
    config_save(archivo);
    config_destroy(archivo);



}//TODO

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
void moverBloque(off_t offsetBloqueOriginal,off_t offsetBloqueDestino){
    
    //asume q checkeaste las cosas en el bit map y q lo vas a modificar, solo copia

    //abro el FS

    int fdBl=open(path_bloques,O_RDWR);
    struct stat sbBl;
    fstat(fdBl,&sbBl);
    char *addrBloques;
    addrBloques=mmap(NULL,sbBl.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fdBl,0);

    //ejecuto tarea
   
    memcpy(addrBloques+(offsetBloqueDestino*interfaz_DialFS.blockSize),addrBloques+(offsetBloqueOriginal*interfaz_DialFS.blockSize),interfaz_DialFS.blockSize);

    //cierro todo
    munmap(addrBloques,sbBl.st_size);
    close(fdBl);

}

void compactarBloquesFSParaQEntreElArchivo(char* nombreDelArchivo,off_t *offsetInicialDelArchivo,int tamanioEnbytesActual){ 
    off_t offsetAux;
    char* nombreAMover;
    int tamanioEnBytesDelArchivo;
    off_t bloqueInicial;
//abro el bitmap
    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);
//abro el FS

    int fdBl=open(path_bloques,O_RDWR);
    struct stat sbBl;
    fstat(fdBl,&sbBl);
    char *addrBloques;
    addrBloques=mmap(NULL,sbBl.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fdBl,0);

//ejecuto tarea LA PUTA MADRE, ACA PIERDO TODAS LAS REFERENCIAS NO SIRVE HAY Q HACERLO POR ARCHIVO :)))))))
    //mueve todos los archivos anteriores y el mismo archivo al principio del archivo
    for(off_t i=0;i<(offsetInicialDelArchivo+(tamanioEnbytesActual/8)+1);i++){
        if(bitarray_test_bit(bitmapAddr,i)){

            offsetAux = buscarBloqueLibre();
            //devuelve char* nombre
            nombreAMover =buscarArchivoConBloqueInicial(i);                             //buscar A q archivo pertenece(funcion)
            moverArchivo(nombreAMover,offsetAux);                                       //mover la cantidad de bloques q tenga ese archivo(funcion mover archivo estaria bn)           
            obtenerInfoDeArchivoTamanio(nombreAMover,&tamanioEnBytesDelArchivo);  //actualiza el i para q siga desde el final del archivo(siempre va a dejar por lo menos un bloque libre al final)
            i=i+(tamanioEnBytesDelArchivo/interfaz_DialFS.blockSize);             //                                                      (excepto q ya este compactado, pero ahi pasa al siguiente archivo q no se mueve y listo)
           
        }
    }//esto aca lee y mueve todos los archivos incluido el q quiere agrandar q estan al principio
    
    //mueve todos los archivos posteriores al final del archivo
    int ultimoBloqueAControlar=interfaz_DialFS.blockCount-1;
    int hayArchivosParaMover=0;
    for(off_t i=(offsetInicialDelArchivo+(tamanioEnbytesActual/8)+1);i<=ultimoBloqueAControlar/*&&q no sea menor a donde empezo*/;i--){
                                                                                              // si es menor va a tirar false en la primera iteracion y listo       
        if(bitarray_test_bit(bitmapAddr,i)){
            hayArchivosParaMover=1;
            nombreAMover =buscarArchivoConBloqueInicial(i);       
            obtenerInfoDeArchivo(nombreAMover,&bloqueInicial,&tamanioEnBytesDelArchivo);
            i=i+(tamanioEnBytesDelArchivo/interfaz_DialFS.blockSize);   
        }

        if(i==ultimoBloqueAControlar&&hayArchivosParaMover){
            if(hayLugarDespuesDelArchivo(1,bloqueInicial+(tamanioEnBytesDelArchivo/8))){
                offsetAux = buscarBloqueLibreDesdeElFinal();
                moverArchivo(nombreAMover,offsetAux);
            }
            ultimoBloqueAControlar=ultimoBloqueAControlar-((tamanioEnBytesDelArchivo/8)+1);
            i=offsetInicialDelArchivo+(tamanioEnbytesActual/8)+1;
            hayArchivosParaMover=0;
        }
            
    }







//cierro todo
    munmap(addrBloques,sbBl.st_size);
    close(fdBl);

    bitarray_destroy(bitmapAddr);


    munmap(addr,sb.st_size);
    close(fd);

}

char* buscarArchivoConBloqueInicial(off_t offsetBloqueInicial){
    DIR *dir=opendir(interfaz_DialFS.pathBaseDialfs);
    struct dirent *entry;
    char *nombre;
    off_t offsetAux;
    while ((entry = readdir(dir)) != NULL)
    {
        nombre=entry->d_name;
        obtenerInfoDeArchivoOffset(nombre,&offsetAux);
        if (offsetAux==offsetBloqueInicial){
            return nombre;
        }

    }
    close(dir);
    return NULL;
}
void moverArchivo(char* nombreArchivo,off_t nuevoBloqueInicialOFinal){
    int fd=open(path_bitmap,O_RDWR);
    struct stat sb;
    fstat(fd,&sb);
    char *addr;
    addr=mmap(NULL,sb.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

    t_bitarray *bitmapAddr=bitarray_create(addr,sb.st_size);
    
    
    
    
    
    int tamanioEnBytes;
    off_t bloqueInicialOriginal;
    obtenerInfoDeArchivo(nombreArchivo,&bloqueInicialOriginal,&tamanioEnBytes);
    int cantidadDeBloques=(tamanioEnBytes/interfaz_DialFS.blockSize)+1;


    cambiarInfoDeArchivo(nombreArchivo,-bloqueInicialOriginal,NULL);

    if(bloqueInicialOriginal<nuevoBloqueInicialOFinal){//caso mueve para "atras"
        
        for(int i=0;i<cantidadDeBloques;i++){
            moverBloque(bloqueInicialOriginal+i,nuevoBloqueInicialOFinal+i);
            bitarray_set_bit(bitmapAddr,nuevoBloqueInicialOFinal+i);
            bitarray_clean_bit(bitmapAddr,bloqueInicialOriginal+i);
        }
        
    }else if(bloqueInicialOriginal>nuevoBloqueInicialOFinal){//caso mueve para "adelante"
        
        off_t bloqueFinalOriginal=bloqueInicialOriginal+cantidadDeBloques-1;
        for(int i=0;i<cantidadDeBloques;i++){
            moverBloque(bloqueFinalOriginal-i,nuevoBloqueInicialOFinal-i);
            bitarray_set_bit(bitmapAddr,nuevoBloqueInicialOFinal-i);
            bitarray_clean_bit(bitmapAddr,bloqueFinalOriginal-i);
        }
        nuevoBloqueInicialOFinal=nuevoBloqueInicialOFinal-cantidadDeBloques+1;

    }

    cambiarInfoDeArchivo(nombreArchivo,nuevoBloqueInicialOFinal,NULL);


    bitarray_destroy(bitmapAddr);
    munmap(addr,sb.st_size);
    close(fd);


}

char* generarPathAArchivoFS(char* nombreArchivo){

    return string_from_format("%s%s",interfaz_DialFS.pathBaseDialfs,nombreArchivo);



}