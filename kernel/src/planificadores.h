#ifndef PLANIFICADORES_H
#define PLANIFICADORES_H

#include <utils.h>
#include <semaphore.h>


//HILOS
pthread_t hilo_largo_plazo;
pthread_t hilo_corto_plazo;

void inicializar_sem_planificadores();
void planificador_largo_plazo();
void planificador_corto_plazo();
void planificar_fifo();
void planificar_round_robin();
void planificar_virtual_round_robin();

#endif