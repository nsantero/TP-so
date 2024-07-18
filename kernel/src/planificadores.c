#include <planificadores.h>
#include <semaforos.h>
#include <configs.h>

//INICIALIZAR PLANIFICADORES

pthread_mutex_t mutexPlanificacion = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condPlanificacion = PTHREAD_COND_INITIALIZER;
int planificacionPausada = 0;

pthread_t hiloQuantum;
t_temporal* tiempoVRR;
int64_t tiempoEjecutando;

int totalProcesosEnSistema(){
    pthread_mutex_lock(&mutexListaReady);
    pthread_mutex_lock(&mutexListaBlocked);
    pthread_mutex_lock(&mutexListaRunning);

    int procesosReady = list_size(lista_READY);
    int procesosBlocked = list_size(lista_BLOCKED);
    int procesosRunning = list_size(lista_RUNNING);

    pthread_mutex_unlock(&mutexListaReady);
    pthread_mutex_unlock(&mutexListaBlocked);
    pthread_mutex_unlock(&mutexListaRunning);

    return procesosReady+procesosBlocked+procesosRunning;
}

void* planificadorNew(){
    while(1){
        sem_wait(&semListaNew); 
        // verifico si la planificación está pausada
        pthread_mutex_lock(&mutexPlanificacion);
        while (planificacionPausada) {
            pthread_cond_wait(&condPlanificacion, &mutexPlanificacion);
        }
        pthread_mutex_unlock(&mutexPlanificacion);

        pthread_mutex_lock(&mutexListaNew);
        int procesosSistema = totalProcesosEnSistema();
        if(!list_is_empty(lista_NEW) && procesosSistema <= configuracionKernel.GRADO_MULTIPROGRAMACION){
            pthread_mutex_lock(&mutexListaReady);
            cambiarAReady(lista_NEW);
            sem_post(&semListaReady);
            pthread_mutex_unlock(&mutexListaReady);
        }
        pthread_mutex_unlock(&mutexListaNew);
    }
    return NULL;
}

void* planificadorReady(){
     while (1) {
        sem_wait(&semListaReady);
        sem_wait(&semListaRunning);

         // Verificar si la planificación está pausada
        pthread_mutex_lock(&mutexPlanificacion);
        while (planificacionPausada) {
            pthread_cond_wait(&condPlanificacion, &mutexPlanificacion);
        }
        pthread_mutex_unlock(&mutexPlanificacion);

        pthread_mutex_lock(&mutexListaReady);
        pthread_mutex_lock(&mutexListaRunning);
        if (!list_is_empty(lista_READY) && list_size(lista_RUNNING) < 1) {
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
        pthread_mutex_unlock(&mutexListaReady);
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

    cambiarARunning(lista_READY);

    PCB* pcbRunnign=list_get(lista_RUNNING, 0);

    pthread_create(&hiloQuantum,NULL, manejadorDeQuantum, &pcbRunnign->quantum);
    paquete_CPU_ejecutar_proceso(pcbRunnign);
    pthread_join(hiloQuantum, NULL);
    paquete_CPU_interrumpir_proceso_fin_quantum(pcbRunnign->PID);
    
}

void *manejadorDeQuantum(void* quantum){
    int quantumProceso = *((int*) quantum);
    log_info(loggerKernel, "el valor obetenido es: %d", quantumProceso);
    usleep(quantumProceso/1000);
    

    return NULL;
}

void comportamientoVRR(){
    tiempoVRR = temporal_create();

    comportamientoRR();
   
}

void cambiarAReady(t_list* cola){
    PCB *proceso = list_remove(cola, 0);
    proceso->estado = READY;
    list_add(lista_READY, proceso);

    return;
}
PCB* cambiarARunning(t_list* lista_READY){
    if (!list_is_empty(lista_READY)) {
    PCB *proceso = list_remove(lista_READY, 0);
    proceso->estado = RUNNING;
    list_add(lista_RUNNING, proceso);
    return proceso;
    }
    return NULL;
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


/*void inicializar_planificadores() {
    t_config* config = config_create("kernel.config");

    // valor del quantum
    if (config_has_property(config, "QUANTUM")) {
        quantum = config_get_int_value(config, "QUANTUM");
    } else {
        printf ("No se encontro el parametro 'QUANTUM' en el archivo de configuracion.\n");
    }
    // tipo de planificador (FIFO o RR)

   if (config_has_property(config, "ALGORITMO_PLANIFICACION")) {
        algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    } else {
        printf ("No se encontro el parametro 'ALGORITMO_PLANIFICACION' en el archivo de configuracion.\n");
    }

    config_destroy(config);
}
*/


/*int hilosPlanificadores(void) {

    pthread_create(&hilo_largo_plazo, NULL, planificador_largo_plazo, NULL);
    pthread_create(&hilo_corto_plazo, NULL, planificador_corto_plazo, NULL);

    pthread_join(hilo_largo_plazo, NULL); // detach no te guarda el valor 
    pthread_join(hilo_corto_plazo, NULL);

    return 0;
}
*/
/*
void planificar_fifo() {
    if (list_is_empty(lista_READY)) { // QUEUE *Lista_ready
        printf ("No hay procesos en la Lista.\n");
        return;
    }
    // variable global con mutex donde definamos que pueda ejecutar wait(mutex)
    // LISTA RUNNING
    sem_wait(&sem_proceso_ejecutando);
    sem_wait(&sem_procesos_ready); 
    list_remove(lista_READY, 0);
    sem_post (&sem_procesos_ready); 
    sem_wait (&sem_procesos_running);
    list_add (lista_RUNNING, 0);
    sem_post (&sem_procesos_running);
    //t_pcb.estado  = RUNNING;
    sem_wait(&sem_proceso_ejecutando);
    //ejecutar_proceso(&t_pcb.PID);
    sem_post(&sem_proceso_ejecutando);

}

 implementación RR
void planificar_round_robin() {
    if (list_is_empty(Lista_de_procesos)) {
        printf ("No hay procesos en la Lista.\n");
        return;
    }

PCB* pcb = list_get(Lista_de_procesos, 0);

int tiempo_ejecucion = ejecutar_proceso(pcb);

if (tiempo_ejecucion < pcb -> quantum) {
    list_remove(Lista_de_procesos, 0); // al haber terminado el proceso dentro del quantum lo elimino de la Lista
    free (pcb); // libero memoria del pcb
} else {
    pcb -> pc += tiempo_ejecucion; //calculo el tiempo que ejecuto y cuanto falta, y lo actualizo en el pc
    list_remove(Lista_de_procesos, 0);
    list_add(Lista_de_procesos, pcb);
}
*/
