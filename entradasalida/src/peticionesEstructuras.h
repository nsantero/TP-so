#ifndef ESTRUCTURAS_PETICIONES_H
#define ESTRUCTURAS_PETICIONES_H

#include <stdint.h>

typedef struct
{
    uint32_t direccion;
    uint8_t tamanio;
    int PID;
}Peticion_Interfaz_STDOUT;

typedef struct
{
    uint32_t direccion;
    uint8_t tamanio;
    int PID;
}Peticion_Interfaz_STDIN;

typedef struct
{
   int unidades_de_trabajo;
   int PID;

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
    uint32_t direcion;
    uint8_t tamanio;
    uint32_t punteroArchivo;
    int PID;

}Peticion_Interfaz_DialFS;




#endif