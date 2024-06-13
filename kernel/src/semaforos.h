#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <semaphore.h>
#include <utils.h>

extern void inicializar_sem_planificadores();

//SEMAFOROS

extern pthread_mutex_t mutexListaNew;
extern pthread_mutex_t mutexListaReady;
extern pthread_mutex_t mutexListaBlocked;
extern pthread_mutex_t mutexListaRunning;
extern pthread_mutex_t mutexListaExit;

extern sem_t semListaNew;
extern sem_t semListaReady;
extern sem_t semListaRunning;

extern sem_t sem_grado_multiprogramacion;
extern sem_t sem_corto_plazo, *sem_largo_plazo;
extern sem_t mutex_detener_planificador;

#endif 

