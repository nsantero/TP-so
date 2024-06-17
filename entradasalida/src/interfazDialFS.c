#include <interfazDialFS.h>

char* path_carpeta_FS = "/home/utnso/tp-2024-1c-File-System-Fanatics/entradasalida/FS/";

char* path_bitmap;
t_config* bitmap;

char* path_bloques;
t_config* bloques;
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
    case CREATE:
        crearNuevoFile(peticion->nombreArchivo);
        break;
    case DELETE:
        /* code */
        break;
    case TRUNCATE:
        /* code */
        break;
    case WRITE:
        /* code */
        break;
    case READ:
        /* code */
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
    txt_close_file(bloquesDat);

    bloques = config_create(path_bloques);
    //TODO no se como hacer q tenga el tamaño q tiene q tener, si llenarlo de ceros o q
    //tmb puedo hacer con config un set bloque x o algo asi no se y q lo lleve el bitmap.dat

    



}
void inicializar_bitmap_dat(Interfaz interfaz){
    
    
    
    path_bitmap=generarPathAArchivoFS("bitmap.dat");
    
    remove(path_bitmap);
    
    FILE* bitmapDat= txt_open_for_append(path_bitmap);
    txt_close_file(bitmapDat);

    //TODO lo mismo, no se como llenarlo
    //ahi con lo de abajo queda pero desordenado
    //no se si esta correcto
    //no se si esta bien hacerlo con lo de config
    // esto esta definitivamente no como lo pensaron ellos
    char* bloque;
    
    bitmap = config_create(path_bitmap);

    for(int i=0;i<interfaz.blockCount;i++){
        bloque=string_from_format("BLOQUE_NRO%d",i);
        config_set_value(bitmap,bloque,"0");
        
    }
    config_save(bitmap);
    

    free(bloque);
    

}


void crearNuevoFile(char* nombre){

    char* path=generarPathAArchivoFS(nombre);
    FILE*archivoNuevo=txt_open_for_append(path);

    txt_close_file(archivoNuevo);

    t_config* archivo=config_create(path);

    char* bloqueInicial=buscarBloqueLibre();

    config_set_value(archivo,"BLOQUE_INICIAL",bloqueInicial);
    config_set_value(archivo,"TAMANIO_ARCHIVO","0");

    config_save(archivo);
    config_destroy(archivo);

    //aca deberia poner el bloque q elije como ocupado en el bit map
    
    config_set_value(bitmap,string_from_format("BLOQUE_NRO%S",bloqueInicial),"1");
    config_save(bitmap);
    

    free(path);
}

char* buscarBloqueLibre(){
    //devuelve el valor numerico como un char* ej :  return "15";

    //TODO aca deberia buscar en el bitmap donde hay un bloque libre para q meta el coso

    char* bloqueAComprar;

    for(int i=0;i<interfaz_DialFS.blockCount;i++){
        bloqueAComprar=string_from_format("BLOQUE_NRO%d",i);
        if(0==config_get_int_value(bitmap,bloqueAComprar)){
            free(bloqueAComprar);
            return string_from_format("%d",i);
        }

    }   
}


char* generarPathAArchivoFS(char* nombreArchivo){

    return string_from_format("%s%s",path_carpeta_FS,nombreArchivo);



}