#ifndef TIPOS_INTERFACES_H
#define TIPOS_INTERFACES_H

typedef struct
{
    char* nombre;
    char* tipoInterfaz;
    int tiempoUnidadTrabajo;
    char* ipKernel;
    char* puertoKernel;
    char* ipMemoria;
    char* puertoMemoria;
    char* pathBaseDialfs;
    int blockSize;
    int blockCount;
    int retrasoCompactacion;

} Interfaz;


#endif