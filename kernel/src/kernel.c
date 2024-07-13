#include <utils.h>
#include <semaforos.h>
#include <kernel.h>
#include <planificadores.h>
#include <conexiones.h>
#include <configs.h>

// LISTA DE ESTADOS

t_list* lista_NEW;
t_list* lista_READY;
t_list* lista_BLOCKED;
t_list* lista_EXIT;
t_list* lista_RUNNING;

void inicializarListas() {
    lista_NEW = list_create();
    lista_READY = list_create(); 
    lista_EXIT = list_create();
    lista_BLOCKED = list_create();
    lista_RUNNING = list_create();
}

//Generar PID

int pid_counter = 1;
int quantum = 2;
char* algoritmo_planificacion = NULL;

// PCB 
    char *pid = NULL;


int pidActual = 0;

int generarPID() {
    pidActual += 1;
    return pidActual;
}

PCB* crearPCB(char* path) {
    printf("Creando PCB... \n");
    PCB* nuevoPCB = malloc(sizeof(PCB)); //reserva de memoria
	 if (nuevoPCB == NULL) {
        // Manejar error de asignación de memoria
        return NULL;
     }

    nuevoPCB -> PID = generarPID(); // asigno pid - al hacerlo incremental me aseguro de que sea único el pid
    nuevoPCB -> cpuRegisters.PC = 0; // contador en 0
    nuevoPCB -> quantum = quantum;//quantum generico tomado de kernel.config
	nuevoPCB -> estado = NEW;
    nuevoPCB->  path = strdup(path); // guardo el path 
    pthread_mutex_lock(&mutexListaNew);
    list_add(lista_NEW, nuevoPCB);
    sem_post(&semListaNew);
    pthread_mutex_unlock(&mutexListaNew);

    paquete_memoria_crear_proceso(nuevoPCB->PID, path);
    
    printf("Tamaño de la lista: %d\n", list_size(lista_NEW));
	// Logueo la creación del PCB
    //char mensaje[100];
    printf("Se creó el PCB del nuevo proceso y se agrego a la lista, PID %d \n", nuevoPCB->PID);
    //log_info(logger, "Se creó el PCB del nuevo proceso, PID %d", nuevoPCB -> PID);
    
    return nuevoPCB;
}
void eliminarProceso(PCB* proceso){
    free(proceso->path);
    free(proceso);
}
void actualizarProceso(PCB* procesoCPU, PCB* procesoKernel){
    procesoKernel->cpuRegisters.PC = procesoCPU->cpuRegisters.PC;
    procesoKernel->cpuRegisters.AX = procesoCPU->cpuRegisters.AX;
    procesoKernel->cpuRegisters.BX = procesoCPU->cpuRegisters.BX;
    procesoKernel->cpuRegisters.CX = procesoCPU->cpuRegisters.CX;
    procesoKernel->cpuRegisters.DX = procesoCPU->cpuRegisters.DX;
    procesoKernel->cpuRegisters.EAX = procesoCPU->cpuRegisters.EAX;
    procesoKernel->cpuRegisters.EBX = procesoCPU->cpuRegisters.EBX;
    procesoKernel->cpuRegisters.ECX = procesoCPU->cpuRegisters.ECX;
    procesoKernel->cpuRegisters.EDX = procesoCPU->cpuRegisters.EDX;
    procesoKernel->cpuRegisters.SI = procesoCPU->cpuRegisters.SI;
    procesoKernel->cpuRegisters.DI = procesoCPU->cpuRegisters.DI;
}
int leer_grado_multiprogramación() {
    return configuracionKernel.GRADO_MULTIPROGRAMACION;
}

#include <stdio.h>
#include <stdlib.h>

// Definiciones de las funciones crear_paquete, agregar_a_paquete, enviar_paquete, eliminar_paquete
// Definiciones de los IDs de paquetes CREAR_PROCESO y DATOS_DEL_PROCESO

void paquete_memoria_crear_proceso(int PID_paquete, char* path_paquete){

    t_paquete *paquete_memoria = crear_paquete(CREAR_PROCESO);
    
    // Agregar el path al paquete
    agregar_entero_a_paquete32(paquete_memoria, PID_paquete);
    agregar_entero_a_paquete32(paquete_memoria, (strlen(path_paquete)+1));
    agregar_string_a_paquete(paquete_memoria, path_paquete);
    
    // Pasar PID y txt a memoria
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);

}

void paquete_memoria_finalizar_proceso(int PID_paquete){

    t_paquete *paquete_memoria = crear_paquete(FINALIZAR_PROCESO);

    // Agregar el path al paquete
    agregar_entero_a_paquete32(paquete_memoria, PID_paquete);

    // Pasar PID y txt a memoria
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);

}
