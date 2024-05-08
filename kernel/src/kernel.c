#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/collections/list.h> //implementación de listas enlazadas que se pueden usar para la cola de procesos
#include <pthread.h>
#include <kernel.h>


//Implementación de PCB

typedef enum {NEW, READY, RUNNING, BLOCKED, EXIT} Estado;

typedef struct {
    int PID; //id del proceso
    int pc; //direccionamiento
    int quantum; // duración del quantum 
    CPU_Registers* cpuRegisters; // puntero a cantidad de registros de la cpu (el valor lo tendría la cpu)
} PCB;


//Inicialización de un nuevo PCB

PCB* crearPCB() {
    PCB* nuevoPCB = malloc(sizeof(PCB)); //reserva de memoria
	 if (nuevoPCB == NULL) {
        // Manejar error de asignación de memoria
        return NULL;
    }
    nuevoPCB -> PID = generarPID(); // asigno pid - al hacerlo incremental me aseguro de que sea único el pid
    nuevoPCB -> pc = 0; // contador en 0
    nuevoPCB -> quantum = quantum;
    nuevoPCB -> estado =  //quantum generico tomado de kernel.config
//	nuevoPCB -> cpuRegisters = crearRegistrosCPU; //voy a la funcion de creacion de registros de la CPU
    return nuevoPCB;
}

int pidActual = 0;
int generarPID() {
    pidActual += 1;
    return pidActual;
}
//como declaro los registros de cpu ¿?

// NUEVO PROCESO 

void nuevoProceso (struct PCB** listaNEW) {
    int PID = generarPID();

    struct PCB* nuevoPCB = crearPCB(PID, NEW);

    agregarALista(listaNEW, nuevoPCB);
}

void agregarALista (struct PCB** lista, struct PCB** nuevoProceso) {
    //evaluo si la lista esta vacia, si esta vacia el nuevo proceso va ser el primero
    if (*lista == NULL) {
        *lista = nuevoProceso;
    }
    else {
        //lo agrego al final de la lista
        struct PCB* ultimoProceso = *lista;
        while (ultimoProceso->next != NULL){
            ultimoProceso = ultimoProceso->next;
        }
        ultimoProceso->next = nuevoProceso;
    }
    nuevoProceso-> next = NULL;
}

// LISTAS DE ESTADOS

struct PCB* NEW = NULL;
struct PCB* READY = NULL;
struct PCB* EXEC = NULL;
struct PCB* BLOCKED = NULL;
struct PCB* EXIT = NULL;

//INICIALIZAR PLANIFICADORES

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

// CONFIGURACIÓN 
#include "../include/kernel.h"


void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion

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

int main(int argc, char* argv[]) {


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



    




