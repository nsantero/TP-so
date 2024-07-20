#ifndef PLANIFICADORES_H
#define PLANIFICADORES_H

#include <utils.h>
#include <semaphore.h>
#include <kernel.h>
#include <semaforos.h>
#include <commons/temporal.h>
#include <conexiones.h>

//HILOS
//pthread_t hilo_largo_plazo;
//pthread_t hilo_corto_plazo;

int totalProcesosEnSistema();
void* planificadorNew();
void* planificadorReady();

void* manejadorDeQuantum(void *quantum);
void cambiarAReady(t_list* cola);
PCB* cambiarARunning(t_list* lista_READY);
PCB* cambiarAExitDesdeRunning(t_list* cola);

void comportamientoFIFO();
void comportamientoRR();
void comportamientoVRR();
void terminarHiloQuantum();

extern pthread_t hiloQuantum;
extern t_temporal* tiempoVRR;
extern int64_t tiempoEjecutando;

extern void inicializar_sem_planificadores();
extern void planificador_largo_plazo();
extern void planificador_corto_plazo();
extern void planificar_fifo();
extern void planificar_round_robin();
extern void planificar_virtual_round_robin();
extern void planificar();
extern void inicializar_planificadores();
extern int hilosPlanificadores(void);

// Iniciar y detener planificacion

extern pthread_mutex_t mutexPlanificacion;
extern pthread_cond_t condPlanificacion;
extern int planificacionPausada;

#endif