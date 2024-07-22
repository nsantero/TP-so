#include <planificadores.h>
#include <semaforos.h>
#include <configs.h>

//INICIALIZAR PLANIFICADORES

pthread_t hiloQuantum;
t_temporal* tiempoVRR;
int64_t tiempoEjecutando;

int totalProcesosEnSistema(){
    pthread_mutex_lock(&mutexListaReady);
    pthread_mutex_lock(&mutexListaBlocked);
    pthread_mutex_lock(&mutexListaBlockedRecursos);
    pthread_mutex_lock(&mutexListaRunning);

    int procesosReady = list_size(lista_READY);
    int procesosBlocked = list_size(lista_BLOCKED);
    int procesosRunning = list_size(lista_RUNNING);
    int procesosBlockedRecursos = list_size(lista_BLOCKED_RECURSOS);

    pthread_mutex_unlock(&mutexListaReady);
    pthread_mutex_unlock(&mutexListaBlocked);
    pthread_mutex_unlock(&mutexListaRunning);
    pthread_mutex_unlock(&mutexListaBlockedRecursos);

    return procesosReady+procesosBlocked+procesosRunning+procesosBlockedRecursos;
}

void* planificadorNew(){
    while(1){
        sem_wait(&semListaNew);

        sem_wait(&semPlaniNew);

        pthread_mutex_lock(&mutexListaNew);
        int procesosSistema = totalProcesosEnSistema();
        if(!list_is_empty(lista_NEW) && procesosSistema <= configuracionKernel.GRADO_MULTIPROGRAMACION){
            pthread_mutex_lock(&mutexListaReady);
            cambiarAReady(lista_NEW);
            sem_post(&semListaReady);
            pthread_mutex_unlock(&mutexListaReady);
        }
        pthread_mutex_unlock(&mutexListaNew);

        sem_post(&semPlaniNew);
    }
    return NULL;
}

void* planificadorReady(){
     while (1) {
        sem_wait(&semPlaniReady);
        sem_wait(&semListaReady);
        sem_wait(&semListaRunning);

        pthread_mutex_lock(&mutexListaReady);
        pthread_mutex_lock(&mutexListaReadyPri);
        pthread_mutex_lock(&mutexListaRunning);
        if ((!list_is_empty(lista_READY) || !list_is_empty(lista_READYPRI))&& list_size(lista_RUNNING) < 1) {
            if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "FIFO")){
                comportamientoFIFO();
            }
            if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "RR")){
                comportamientoRR();
            }
            if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
                comportamientoVRR();
            }
        }
        pthread_mutex_unlock(&mutexListaRunning);
        pthread_mutex_unlock(&mutexListaReadyPri);
        pthread_mutex_unlock(&mutexListaReady);
        sem_post(&semPlaniReady);
    }
    return NULL;
}

void comportamientoFIFO(){
    PCB* proceso = cambiarARunning(lista_READY);
    if (proceso) {
        paquete_CPU_ejecutar_proceso(proceso);
    }
}

void comportamientoRR(){

    PCB* pcbRunnign;

    pcbRunnign=cambiarARunningVRR();

    pcbRunnign=list_get(lista_RUNNING, 0);

    pthread_create(&hiloQuantum,NULL, manejadorDeQuantum, &pcbRunnign->quantum);
    paquete_CPU_ejecutar_proceso(pcbRunnign);
    pthread_join(hiloQuantum, NULL);
    paquete_CPU_interrumpir_proceso_fin_quantum(pcbRunnign->PID);
    
}

void *manejadorDeQuantum(void* quantum){
    int quantumProceso = *((int*) quantum);
    usleep(quantumProceso*1000);
    

    return NULL;
}

void comportamientoVRR(){
    tiempoVRR = temporal_create();

    comportamientoRR();
    temporal_stop(tiempoVRR);
	tiempoEjecutando = temporal_gettime(tiempoVRR);
	temporal_destroy(tiempoVRR);
   
}

void cambiarAReady(t_list* cola){
    PCB *proceso = list_remove(cola, 0);
    proceso->estado = READY;
    list_add(lista_READY, proceso);

    return;
}
PCB* cambiarARunning(t_list* lista){
    if (!list_is_empty(lista)) {
        PCB *proceso = list_remove(lista, 0);
        proceso->estado = RUNNING;
        list_add(lista_RUNNING, proceso);
        return proceso;
    }
    return NULL;
}
PCB* cambiarARunningVRR(){
    PCB* proceso;
    if(!list_is_empty(lista_READYPRI)){
        proceso=cambiarARunning(lista_READYPRI);
    }
    else{
        proceso=cambiarARunning(lista_READY);
    }

    return proceso;
}
PCB* cambiarAExitDesdeRunning(t_list* cola){
    PCB *proceso = list_remove(cola, 0);
    proceso->estado = EXIT;
    list_add(lista_EXIT, proceso);

    return proceso;
}

void terminarHiloQuantum(){
    pthread_mutex_lock(&mutexHiloQuantum);
    pthread_cancel(hiloQuantum);
    pthread_mutex_unlock(&mutexHiloQuantum);
}