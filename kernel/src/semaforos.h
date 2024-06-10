#ifndef SEMAFORO_H
#define SEMAFORO_H

#include <semaphore.h>
#include <utils.h>
#include <conexiones.h>

//SEMAFOROS
sem_t *sem_grado_multiprogramacion;
sem_t *sem_corto_plazo, *sem_largo_plazo;
sem_t *mutex_detener_planificador;
sem_t *sem_procesos_new;
sem_t *sem_procesos_ready;
sem_t *sem_procesos_running;
sem_t *sem_proceso_ejecutando;

// FUNCIONES

void inicializar_sem_planificadores();



#endif 

