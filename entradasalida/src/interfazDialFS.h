#ifndef INTEFAZ_DIALFS_H
#define INTEFAZ_DIALFS_H

#include <peticionesEstructuras.h>
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
#include <sys/types.h>
#include <unistd.h>
#include <commons/bitarray.h>
#include <dirent.h>
#include <shared.h>
#include <entradasalida.h>
#include <loggersIO.h>


extern Interfaz interfaz_DialFS;
extern t_list * cola_procesos_DialFS;
extern pthread_mutex_t mutex_cola_DialFS;;
extern sem_t sem_hay_en_DialFS;



typedef struct 
{
    char* nodo_path;
    off_t nodo_offsetBloqueInicial;
    int nodo_tamanioEnBytes;
}NodoFs;




Interfaz generarNuevaInterfazDialFS(char* nombre,t_config* configuracion);

void EJECUTAR_INTERFAZ_DialFS();

void* manejo_interfaz_DialFS();




void inicializar_sem_cola_DialFS();



void inicializar_bloques_dat(Interfaz interfaz);
void inicializar_bitmap_dat(Interfaz interfaz);


void crearNuevoFile(Peticion_Interfaz_DialFS* peticion);

void borrarFile(Peticion_Interfaz_DialFS* peticion);
void truncarArchivo(Peticion_Interfaz_DialFS* peticion);
void escribirEnArchivo(Peticion_Interfaz_DialFS* peticion);
void leerDelArchivo(Peticion_Interfaz_DialFS* peticion);
int hayLugarDespuesDelArchivo(int cantBloques,off_t ultimoBloqueDelArchivo);
int existenBloquesDisponibles(int bloquesNecesarios);


off_t buscarBloqueLibre();
off_t buscarBloqueLibreDesdeElFinal();
char* obtenerInfoDeArchivo(char* nombreArchivo,off_t* offset,int* tamanioEnBytes);
void obtenerInfoDeArchivoOffset(t_config* archivo,off_t* offset);
void obtenerInfoDeArchivoTamanio(t_config* archivo,int* tamanioEnBytes);
void cambiarInfoDeArchivo(char* nombreArchivo,off_t offset,int tamanioEnBytes);

void liberarBloque(off_t offset);
void ocuparBloque(off_t offset);
void moverBloque(off_t offsetBloqueOriginal,off_t offsetBloqueDestino);

void compactarBloquesFSParaQEntreElArchivo(char* nombreDelArchivo,off_t offsetInicialDelArchivo,int tamanioEnbytesActual);

char* buscarArchivoConBloqueInicial(off_t offsetBloqueInicial);
void moverArchivo(char* nombreArchivo,off_t nuevoBloqueInicialOFinal);

void enviarBufferAMemoria();
void* recibirBufferDeMememoria();

char* generarPathAArchivoFS(char* nombreArchivo);




#endif