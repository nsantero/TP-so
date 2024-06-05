#ifndef PLANIFICADORES_H
#define PLANIFICADORES_H

#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/collections/list.h> //implementación de listas enlazadas que se pueden usar para la cola de procesos
#include <pthread.h>
#include <commons/log.h>
#include "semaphore.h"

//HILOS
extern pthread_t hilo_largo_plazo;
extern pthread_t hilo_corto_plazo;

//SEMAFOROS
extern sem_t *sem_grado_multiprogramacion;
extern sem_t *sem_corto_plazo, *sem_largo_plazo;
extern sem_t *mutex_detener_planificador;
extern sem_t *sem_procesos_new;
extern sem_t *sem_procesos_ready;
extern sem_t *sem_procesos_running;
extern sem_t *sem_proceso_ejecutando;


void inicializar_sem_planificadores();

void planificador_largo_plazo();

void planificador_corto_plazo();

void planificar_fifo();

void planificar_round_robin();

void planificar_virtual_round_robin();


#endif