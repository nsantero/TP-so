#include <planificadores.h>
#include <pthread.h>
#include <stdio.h>
#include <kernel.h>

int main() {
    pthread_t hilo_largo_plazo;
    pthread_t hilo_corto_plazo;

    pthread_create(&hilo_largo_plazo, NULL, largo_plazo, NULL);
    pthread_create(&hilo_corto_plazo, NULL, corto_plazo, NULL);

    pthread_join(hilo_largo_plazo, NULL);
    pthread_join(hilo_corto_plazo, NULL);

    return 0;
}

int leer_grado_multiprogramación() {
    FILE* archivo = fopen ("kernel.config", "r");
    int GRADO_MULTIPROGRAMACION;
}


// PLANIFICADOR LARGO PLAZO

void* largo_plazo(void* arg) {
    //desarrollar
}

// PLANIFICADOR CORTO PLAZO

void* corto_plazo(void* arg) {
    if(strcmp(algoritmo_planificacion, "FIFO") == 0) {
        planificar_fifo();
    } else if (strcmp(algoritmo_planificacion, "RR") == 0) {
        planificar_round_robin();
    } else if (strcmp(algoritmo_planificacion, "VRR") == 0 ) {
        printf ("Algoritmo de planificacion no conocido: %s\n", algoritmo_planificacion);
    }
}

void planificar_fifo() {
    if (list_is_empty(cola_de_procesos)) {
        printf ("No hay procesos en la cola.\n");
        return;
    }

    PCB* pcb= list_remove(cola_de_procesos, 0);
    ejecutar_proceso(pcb); //falta implementar esta funcion
    free(pcb); //libero memoria
}

// implementación RR
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
}