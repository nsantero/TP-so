#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/collections/list.h> //implementación de listas enlazadas que se pueden usar para la cola de procesos
#include <pthread.h>
#include "kernel.h"
#include <semaphore.h>
#include "planificadores.h"
#include "semaforos.h"
#include <shared.h>

//Generar PID

int pidActual = 0;

int generarPID() {
    pidActual += 1;
    return pidActual;
}

void inicializar_sem_planificadores()
{
	sem_corto_plazo = malloc(sizeof(sem_t));
	sem_init(sem_corto_plazo, 0, 0);

	sem_largo_plazo = malloc(sizeof(sem_t));
	sem_init(sem_largo_plazo, 0, 0);

	sem_grado_multiprogramacion = malloc(sizeof(sem_t));
	sem_init(sem_grado_multiprogramacion, 0, leer_grado_multiprogramación());

	mutex_detener_planificador = malloc(sizeof(sem_t));
	sem_init(mutex_detener_planificador, 0, 1);

    sem_procesos_new = malloc (sizeof(sem_t));
    sem_init(sem_procesos_new, 0, 1);

    sem_procesos_ready = malloc (sizeof(sem_t));
    sem_init(sem_procesos_ready, 0, 1);

    sem_procesos_running = malloc (sizeof(sem_t));
    sem_init(sem_procesos_running, 0, 1);

    sem_proceso_ejecutando = malloc (sizeof(sem_t));
    sem_init(sem_proceso_ejecutando, 0, 0);
}
//Inicialización de un nuevo PCB

PCB* crearPCB() {
    PCB* nuevoPCB = malloc(sizeof(PCB)); //reserva de memoria
	 if (nuevoPCB == NULL) {
        // Manejar error de asignación de memoria
        return NULL;
    }
    nuevoPCB -> PID = generarPID(); // asigno pid - al hacerlo incremental me aseguro de que sea único el pid
    nuevoPCB -> pc = 0; // contador en 0
    nuevoPCB -> quantum = quantum;//quantum generico tomado de kernel.config
	nuevoPCB -> estado = NEW;
	list_add(lista_NEW, 0);

	// Logueo la creación del PCB
	char mensaje[100];
	sprintf(mensaje, "Se creo el PCB del nuevo proceso, PID");
	log_info(logger, mensaje);
	}



//INICIALIZAR PLANIFICADORES

void_inicializar_planificadores() {
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

    // inicializamos lista de estados

    lista_NEW = list_create(); //queue
	lista_READY = list_create();
	lista_EXIT = list_create();
	lista_BLOCKED = list_create();
}


void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion
    printf("Holis");
	if (config == NULL) {
		perror("Archivo de configuracion no encontrado");
		exit(-1);
	}

	config_valores.puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
	config_valores.ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	config_valores.puerto_memoria= config_get_string_value(config,"PUERTO_MEMORIA");
	config_valores.ip_cpu = config_get_string_value(config,"IP_CPU"); 
	config_valores.puerto_cpu_dispatch = config_get_string_value(config,"PUERTO_CPU_DISPATCH");
    config_valores.puerto_cpu_interrupt = config_get_string_value(config,"PUERTO_CPU_INTERRUPT");
    config_valores.algoritmo_planificacion = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
    config_valores.quantum = config_get_int_value(config,"QUANTUM");
    config_valores.recursos =  config_get_array_value(config, "RECURSOS");
	config_valores.instancias_recursos = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    config_valores.grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
}

int iniciar_comunicacion(int argc, char* argv[]) {


    logger = log_create("../kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	log_info(logger, "Se creo el log!");

	cargar_configuracion("/home/utnso/tp-2024-1c-File-System-Fanatics/kernel/kernel.config");

	// Conecto kernel con cpu y memoria
	cpu_dispatch_fd = crear_conexion(logger,"CPU_DISPATCH",config_valores.ip_cpu,config_valores.puerto_cpu_dispatch);
	log_info(logger, "Me conecte a cpu (dispatch)");
    cpu_interrupt_fd = crear_conexion(logger,"CPU_INTERRUPT",config_valores.ip_cpu,config_valores.puerto_cpu_interrupt);
	log_info(logger, "Me conecte a cpu (interrupt)");
    memoria_fd = crear_conexion(logger,"MEMORIA",config_valores.ip_memoria,config_valores.puerto_memoria);
	log_info(logger, "Me conecte a memoria");

	// envio mensajes
	enviar_mensaje("soy Kernel", memoria_fd);
    enviar_mensaje("soy Kernel", cpu_dispatch_fd);

	//levanto servidor
	server_fd = iniciar_servidor(logger,server_name ,IP, config_valores.puerto_escucha);
	log_info(logger, "Servidor listo para recibir al cliente");

	t_paquete *paquete = crear_paquete(CREAR_PROCESO);

	PCB t_pcb;

	// Agregar el path al paquete
	agregar_a_paquete(paquete,&t_pcb.PID,sizeof(int));
	agregar_a_paquete(paquete, "instruccion.txt", strlen("instruccion.txt") + 1);
	
	// pasar PID y txt a memoria
	enviar_paquete(paquete, memoria_fd);
	eliminar_paquete(paquete);

	//t_paquete *paquete = crear_paquete(DATOS_DEL_PROCESO);

	agregar_a_paquete(paquete, &t_pcb.PID, sizeof(int));
	agregar_a_paquete(paquete, &t_pcb.pc, sizeof(int));


	//Paso el PID y PC a la CPU
	
	enviar_paquete(paquete, cpu_dispatch_fd);
	eliminar_paquete(paquete);

	// espero mensjaes de e/s
    while(server_escuchar(server_fd));



    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////// PROCESO CONEXION //////////////////////////////////////////////////////////

static void procesar_conexion(void *void_args) {
	int *args = (int*) void_args;
	int cliente_socket = *args;
	op_code cop;
    
	while (cliente_socket != -1) {
		if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
			log_info(logger, ANSI_COLOR_BLUE"El cliente se desconecto de %s server", server_name);
			return;
		}
		switch (cop) {
		case MENSAJE:{

			char* mensaje = recibir_mensaje(cliente_socket);
            log_info(logger, ANSI_COLOR_YELLOW"Me llegó el mensaje: %s", mensaje);
                
            free(mensaje); // Liberar la memoria del mensaje recibido

			break;
            }
		case PAQUETE:

			///// IMPLEMENTAR
			
			break;
		
        default:
            printf("Error al recibir  %d \n", cop);
            break;
	return;
        }
    }
}

int server_escuchar(int fd_memoria) {
	
	int cliente_socket = esperar_cliente(logger, server_name, fd_memoria);

	if (cliente_socket != -1) {
		pthread_t hilo;
		pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) &cliente_socket);
		pthread_detach(hilo);
		return 1;
	}

	return 0;
}

void ejecutar_proceso() {

    //t_paquete *paquete = crear_paquete(EJECUTAR_PROCESO);

	//agregar_a_paquete(paquete, &t_pcb.PID, sizeof(int));


	//Paso el PID a CPU para que identifique el proceso a ejecutar
	
	//enviar_paquete(paquete, cpu_dispatch_fd);
	//eliminar_paquete(paquete);
}

//Hilos

void* largo_plazo(void* arg) {
    if (list_size(lista_NEW) != 0)  {

        sem_wait(sem_grado_multiprogramacion); // me fijo que el grado de multiprogramacion este ok
        sem_wait(sem_procesos_new);
        sem_wait(sem_procesos_ready);
        list_remove(lista_NEW, 0); // el 0 indica que se elimina el primer elemento de la lista(como el proceso a analizar es el primero, va estar bien quitarlo de NEW)
        list_add(lista_READY, 0); 
        PCB* Estado = "READY"; // lo agrega al comienzo de la lista, cambiarlo en base a queue
        sem_post(sem_procesos_new);
        sem_post(sem_procesos_ready);
    }
    pthread_exit(NULL);
}

// PLANIFICADOR CORTO PLAZO
// Evaluo que tipo de planificador se debe implementar

void* corto_plazo(void* arg) { // READY - RUNNING - BLOCKED

    if (list_size (lista_READY) != 0) {

    if(strcmp(algoritmo_planificacion, "FIFO") == 0) {
        planificar_fifo();
    } else if (strcmp(algoritmo_planificacion, "RR") == 0) {
        planificar_round_robin();
    } else if (strcmp(algoritmo_planificacion, "VRR") == 0 ) {
        printf ("Algoritmo de planificacion no conocido: %s\n", algoritmo_planificacion);
    }
    }
}

int hilos(void) {

    pthread_create(&hilo_largo_plazo, NULL, largo_plazo, NULL);
    pthread_create(&hilo_corto_plazo, NULL, corto_plazo, NULL);

    pthread_join(hilo_largo_plazo, NULL); // detach no te guarda el valor 
    pthread_join(hilo_corto_plazo, NULL);

    return 0;
}

int leer_grado_multiprogramación() {
    FILE* archivo = fopen ("kernel.config", "r");
    int GRADO_MULTIPROGRAMACION;
}

bool permitePasarAREady() {
    leer_grado_multiprogramacion() > list_size(lista_READY);
}


// PLANIFICADOR LARGO PLAZO 
// El proceso pasa de NEW a READY
// Informativo: sem_wait bloquea el hilo si el semaforo se encuentra en 0 o menor a 0. Si el valor del
//semaforo es 1, decrementa el valor (-1) y ejecuta el hilo.


PCB t_pcb;
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
    PCB* Estado = "RUNNING";
    sem_wait(sem_proceso_ejecutando);
    ejecutar_proceso(&t_pcb.PID);
    sem_post(sem_proceso_ejecutando);

}



/*

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


*/