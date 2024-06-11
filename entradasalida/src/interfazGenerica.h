#ifndef INTEFAZ_GENERICA_H
#define INTEFAZ_GENERICA_H


#include <stdint.h>
#include <tiposInterfaces.h>
#include <commons/config.h>
#include <pthread.h>
#include <semaphore.h>

extern t_list * cola_procesos_ig;

extern pthread_mutex_t mutex_cola_ig;

extern sem_t sem_hay_en_cola;

typedef struct
{
   int unidades_de_trabajo;
   int PID;

} Peticion_Interfaz_Generica;

/**
 * @fn generarNuevaInterfazGenerica
 * @brief Duevuelve una interfaz a partir de un archivo de configuracion
 */


Interfaz generarNuevaInterfazGenerica(char* nombre,char* pathConfiguracion);


/**
 * @fn manejarPeticionInterfazGenerica
 * @brief espera una cantidad de tiempo
 */

void manejarPeticionInterfazGenerica(unsigned unidadesAEsperar,Interfaz interfaz);

#endif