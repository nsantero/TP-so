#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <semaphore.h>
#include <utils.h>

extern void inicializar_sem_planificadores();
void destruirSemaforos();

//SEMAFOROS

extern pthread_mutex_t mutexListaNew;
extern pthread_mutex_t mutexListaReady;
extern pthread_mutex_t mutexListaReadyPri;
extern pthread_mutex_t mutexListaBlocked;
extern pthread_mutex_t mutexListaBlockedRecursos;
extern pthread_mutex_t mutexListaRunning;
extern pthread_mutex_t mutexListaExit;

extern pthread_mutex_t mutexLogger;
extern pthread_mutex_t mutexHiloQuantum;

extern sem_t semListaNew;
extern sem_t semListaReady;
extern sem_t semListaRunning;

extern sem_t semPlaniNew;
extern sem_t semPlaniReady;
extern sem_t semPlaniBlocked;
extern sem_t semPlaniRunning;
extern sem_t semPlaniReadyClock;
extern sem_t semPlaniBlockedRecursos;
extern sem_t semPlaniBlockedPrioridad;


extern sem_t sem_grado_multiprogramacion;
extern sem_t mutex_detener_planificador;

#endif 

