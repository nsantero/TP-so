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

PCB* crearPCB() {
    printf("Creando PCB... \n");
    PCB* nuevoPCB = malloc(sizeof(PCB)); //reserva de memoria
	 if (nuevoPCB == NULL) {
        // Manejar error de asignación de memoria
        return NULL;
     }

    nuevoPCB -> PID = generarPID(); // asigno pid - al hacerlo incremental me aseguro de que sea único el pid
    nuevoPCB -> pc = 0; // contador en 0
    nuevoPCB -> quantum = quantum;//quantum generico tomado de kernel.config
	nuevoPCB -> estado = NEW;
    pthread_mutex_lock(&mutexListaNew);
    list_add(lista_NEW, nuevoPCB);
    sem_post(&semListaNew);
    pthread_mutex_unlock(&mutexListaNew);
    
    printf("Tamaño de la lista: %d\n", list_size(lista_NEW));
	// Logueo la creación del PCB
    //char mensaje[100];
    printf("Se creó el PCB del nuevo proceso y se agrego a la lista, PID %d \n", nuevoPCB->PID);
    //log_info(logger, "Se creó el PCB del nuevo proceso, PID %d", nuevoPCB -> PID);
    
    return nuevoPCB;
}

int leer_grado_multiprogramación() {
    return configuracionKernel.GRADO_MULTIPROGRAMACION;
}

/*void* largo_plazo(void* arg) {
    if (list_size(lista_NEW) != 0)  {

        sem_wait(sem_grado_multiprogramacion); // me fijo que el grado de multiprogramacion este ok
        sem_wait(sem_procesos_new);
        sem_wait(sem_procesos_ready);
        list_remove(lista_NEW, 0); // el 0 indica que se elimina el primer elemento de la lista(como el proceso a analizar es el primero, va estar bien quitarlo de NEW)
        list_add(lista_READY, 0); 
        t_pcb.estado = READY; // lo agrega al comienzo de la lista, cambiarlo en base a queue
        sem_post(sem_procesos_new);
        sem_post(sem_procesos_ready);
    }
    pthread_exit(NULL);
}
*/

/*
// LARGO PLAZO PASA DE NEW A READY
void planificar_largo_plazo(PCB* proceso_recibido, char* path_recibido) {
    if (list_size(lista_NEW) < leer_grado_multiprogramación()  ) {
    printf("grado de multiprogramacion: %d\n", leer_grado_multiprogramación());
    list_remove_element(lista_NEW, proceso_recibido->PID);
    list_add(lista_READY, proceso_recibido->PID);
    printf("Se movio a ready el proceso %d\n", proceso_recibido->PID);
    paquete_crear_proceso(proceso_recibido->PID, path_recibido, proceso_recibido->pc );
    printf("Se envio el paquete a memoria y cpu %d\n", proceso_recibido->PID);
    }
    else {
        printf("No se puede pasar el proceso a ready");
    }


}

// PLANIFICADOR CORTO PLAZO
// Evaluo que tipo de planificador se debe implementar

void planificar_corto_plazo(void* arg) { // READY - RUNNING - BLOCKED

    if (list_size (lista_READY) != 0) {

    if(strcmp(algoritmo_planificacion, "FIFO") == 0) {
        planificar_fifo();
    } //else if (strcmp(algoritmo_planificacion, "RR") == 0) {
        //planificar_round_robin();
    //} else if (strcmp(algoritmo_planificacion, "VRR") == 0 ) {
    //    printf ("Algoritmo de planificacion no conocido: %s\n", algoritmo_planificacion);
    }
    }

*/
void paquete_crear_proceso(int PID_paquete, char* path_paquete, int pc_paquete){
    t_paquete *paquete_memoria = crear_paquete(CREAR_PROCESO);

	//Agregar el path al paquete
	agregar_a_paquete(paquete_memoria,&PID_paquete,sizeof(int));
	agregar_a_paquete(paquete_memoria, path_paquete, strlen(path_paquete) + 1);
	
	// pasar PID y txt a memoria
	enviar_paquete(paquete_memoria, memoria_fd);
	eliminar_paquete(paquete_memoria);

	t_paquete *paquete_cpu = crear_paquete(DATOS_DEL_PROCESO);

	agregar_a_paquete(paquete_cpu, &PID_paquete, sizeof(int));
	agregar_a_paquete(paquete_cpu, &pc_paquete, sizeof(int));


	//Paso el PID y PC a la CPU
	
	enviar_paquete(paquete_cpu, cpu_dispatch_fd);
	eliminar_paquete(paquete_cpu);

	//levanto servidor
    /*
	server_fd = iniciar_servidor(logger,server_name ,IP, config_valores.puerto_escucha);
	//log_info(logger, "Servidor listo para recibir al cliente");

    printf("Se envio el paquete a cpu y memoria \n");
    while(server_escuchar(server_fd));
	*/

}








