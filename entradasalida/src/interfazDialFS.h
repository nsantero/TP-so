#ifndef INTEFAZ_DIALFS_H
#define INTEFAZ_DIALFS_H

#include <stdint.h>
#include <tiposInterfaces.h>
#include <commons/config.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <commons/bitarray.h>


char* path_carpeta_FS;
extern Interfaz interfaz_DialFS;
extern t_list * cola_procesos_DialFS;
extern pthread_mutex_t mutex_cola_DialFS;;
extern sem_t sem_hay_en_DialFS;

typedef enum {
    DFS_CREATE,
    DFS_DELETE,
    DFS_TRUNCATE,
    DFS_WRITE,
    DFS_READ
}OperacionesDeDialFS;



typedef struct 
{

    OperacionesDeDialFS operacion;
    char* nombreArchivo;
    uint32_t direcion;
    uint8_t tamanio;
    uint32_t punteroArchivo;
    int PID;

}Peticion_Interfaz_DialFS;

Interfaz generarNuevaInterfazDialFS(char* nombre,char* pathConfiguracion);

void EJECUTAR_INTERFAZ_DialFS();

void* manejo_interfaz_DialFS();




#endif