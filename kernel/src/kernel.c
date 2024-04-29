#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <commons/config.h>
#include <commons/collections/list.h> //implementación de listas enlazadas que se pueden usar para la cola de procesos
#include <pthread.h>

int main() {
    inicializar();

    while (list_is_empty(cola_de_procesos)){
        planificar();
    }
    return 0;
}

//Implementación de PCB

typedef struct {
    int PID; //id del proceso
    int pc; //direccionamiento
    int quantum; // duración del quantum 
    int cpuRegisters[16]; //registros de la cpu - el valor lo puse genérico, no se si estaria bien (help)
} PCB;

int pid_counter = 1; //contador del pid, lo inicializo en 1
int quantum; 
char* algoritmo_planificacion; 

t_list* cola_de_procesos;

void_inicializar() {
    t_config* config = config_create("kernel.config");

    // valor del quantum
    if (config_has_property(config, "QUANTUM")) {
        quantum = config_get_int_value(config, "QUANTUM");
    } else {
        printf ("No se encontro el parametro 'QUANTUM' en el archivo de configuracion.\n");
    }

    config_destroy(config);

    // tipo de planificador (FIFO o RR)

    if (config_has_property(config, "ALGORITMO_PLANIFICACION")) {
        algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    } else {
        printf ("No se encontro el parametro 'ALGORITMO_PLANIFICACION' en el archivo de configuracion.\n");
    }

    // inicilizamos la cola de procesos

    cola_de_procesos = list_create();
}

PCB* crearPCB() {
    PCB* nuevoPCB = malloc(sizeof(PCB)); //reserva de memoria
    nuevoPCB -> PID = pid_counter++; // asigno pid - al hacerlo incremental me aseguro de que sea único el pid
    nuevoPCB -> pc = 0; // contador en 0
    nuevoPCB -> quantum = quantum; //quantum generico tomado de kernel.config
    for (int i = 0; i<16; i++) {
        nuevoPCB -> cpuRegisters[i] = 0 ;
    }
    return nuevoPCB;
}

// implementación FIFO (el primero en la cola es el primero en salir y ser ejecutado)
// implementación RR

void planificar() {
    if(strcmp(algoritmo_planificacion, "FIFO") == 0) {
        planificar_fifo();
    } else if (strcmp(algoritmo_planificacion, "RR") == 0) {
        planificar_round_robin();
    } else {
        printf ("Algoritmo de planificacion no conocido: %s\n", algoritmo_planificacion);
    }
}

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

void planificar_fifo() {
    if (list_is_empty(cola_de_procesos)) {
        printf ("No hay procesos en la cola.\n");
        return;
    }

    PCB* pcb= list_remove(cola_de_procesos, 0);
    ejecutar_proceso(pcb); //falta implementar esta funcion
    free(pcb); //libero memoria
}


