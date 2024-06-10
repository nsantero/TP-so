#include <planificadores.h>


//INICIALIZAR PLANIFICADORES

void inicializar_planificadores() {
    t_config* config = config_create("kernel.config");

    // valor del quantum
    if (config_has_property(config, "QUANTUM")) {
        quantum = config_get_int_value(config, "QUANTUM");
    } else {
        printf ("No se encontro el parametro 'QUANTUM' en el archivo de configuracion.\n");
    }
    // tipo de planificador (FIFO o RR)

/*    if (config_has_property(config, "ALGORITMO_PLANIFICACION")) {
        algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    } else {
        printf ("No se encontro el parametro 'ALGORITMO_PLANIFICACION' en el archivo de configuracion.\n");
    }

    config_destroy(config);

}*/

int hilosPlanificadores(void) {

    pthread_create(&hilo_largo_plazo, NULL, planificador_largo_plazo, NULL);
    pthread_create(&hilo_corto_plazo, NULL, planificador_corto_plazo, NULL);

    pthread_join(hilo_largo_plazo, NULL); // detach no te guarda el valor 
    pthread_join(hilo_corto_plazo, NULL);

    return 0;
}

void planificar_fifo() {
    if (list_is_empty(lista_READY)) { // QUEUE *cola_ready
        printf ("No hay procesos en la cola.\n");
        return;
    }
    // variable global con mutex donde definamos que pueda ejecutar wait(mutex)
    // LISTA RUNNING
    sem_wait(sem_proceso_ejecutando);
    sem_wait (sem_procesos_ready); 
    list_remove(lista_READY, 0);
    sem_post (sem_procesos_ready); 
    sem_wait (sem_procesos_running);
    list_add (lista_RUNNING, 0);
    sem_post (sem_procesos_running);
    t_pcb.estado  = RUNNING;
    sem_wait(sem_proceso_ejecutando);
    ejecutar_proceso(&t_pcb.PID);
    sem_post(sem_proceso_ejecutando);

}
void planificar_round_robin() {
   } 

/* implementación RR
void planificar_round_robin() {
    if (list_is_empty(cola_de_procesos)) {
        printf ("No hay procesos en la cola.\n");
        return;
    }

PCB* pcb = list_get(cola_de_procesos, 0);

int tiempo_ejecucion = ejecutar_proceso(pcb);

if (tiempo_ejecucion < pcb -> quantum) {
    list_remove(cola_de_procesos, 0); // al haber terminado el proceso dentro del quantum lo elimino de la cola
    free (pcb); // libero memoria del pcb
} else {
    pcb -> pc += tiempo_ejecucion; //calculo el tiempo que ejecuto y cuanto falta, y lo actualizo en el pc
    list_remove(cola_de_procesos, 0);
    list_add(cola_de_procesos, pcb);
}
*/
