#ifndef ESTRUCTURAS_PETICIONES_H
#define ESTRUCTURAS_PETICIONES_H

#include <stdint.h>
#include <commons/collections/list.h>

typedef struct {
    uint32_t numero_frame;
    uint32_t desplazamiento;
    uint32_t bytes;
}Direccion_fisicaIO;

typedef struct
{
    uint32_t tamanio;
    int PID;
    char *nombre_interfaz;
    Direccion_fisicaIO direccion_fisica;
    t_list* frames; //lista de enteros, nro de frames
    uint32_t tamPag;
}Peticion_Interfaz_STDOUT;

typedef struct
{
    uint32_t tamanio;
    int PID;
    char *nombre_interfaz;
    Direccion_fisicaIO direccion_fisica;
    t_list* frames; //lista de enteros, nro de frames
    uint32_t tamPag;
}Peticion_Interfaz_STDIN;


typedef struct
{
   int unidades_de_trabajo;
   int PID;
   char *nombre_interfaz;
} Peticion_Interfaz_Generica;

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
    uint32_t direcion;// puede usar este en el truncate para el tamaño
    uint32_t tamanio;
    //uint32_t tamanioExtendido; ?? va? para truncate //TODO
    uint32_t punteroArchivo;
    int PID;
    char *nombre_interfaz;
}Peticion_Interfaz_DialFS;

typedef enum {
    T_GENERICA,
    T_STDIN,
    T_STDOUT,
    T_DFS,
}Tipos_Interfaz;

int obtenerTipoConString(char* tipo);


#endif