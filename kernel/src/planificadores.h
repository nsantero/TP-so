#ifndef PLANIFICADORES_H
#define PLANIFICADORES_H

#include <utils.h>
#include <semaphore.h>
#include <kernel.h>
#include <semaforos.h>
#include <conexiones.h>


//HILOS
pthread_t hilo_largo_plazo;
pthread_t hilo_corto_plazo;

void inicializar_sem_planificadores();
void planificador_largo_plazo();
void planificador_corto_plazo();
void planificar_fifo();
void planificar_round_robin();
void planificar_virtual_round_robin();
void planificar();
void inicializar_planificadores()

#endif