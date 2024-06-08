#include <utils.h>
#include <kernel.h>

    char *linea;
    char *path_script = NULL;
    char *path_proceso = NULL;
    char *pid = NULL;
    char *valor_multiprogramacion = NULL;
    char *archivo_configuracion = "/home/utnso/tp-2024-1c-File-System-Fanatics/kernel/kernel.config";


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
    list_add(lista_NEW, nuevoPCB->PID);
    printf("Tamaño de la lista: %d\n", list_size(lista_NEW));
	// Logueo la creación del PCB
    //char mensaje[100];
    printf("Se creó el PCB del nuevo proceso y se agrego a la lista, PID %d \n", nuevoPCB->PID);
    //log_info(logger, "Se creó el PCB del nuevo proceso, PID %d", nuevoPCB -> PID);
    
    return nuevoPCB;
}

PCB t_pcb;
void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion
	if (config == NULL) {
		perror("Archivo de configuracion no encontrado");
		exit(-1);
	}

    config_valores.puerto_escucha = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));
    config_valores.ip_memoria = strdup(config_get_string_value(config, "IP_MEMORIA"));
    config_valores.puerto_memoria = strdup(config_get_string_value(config, "PUERTO_MEMORIA"));
    config_valores.ip_cpu = strdup(config_get_string_value(config, "IP_CPU"));
    config_valores.puerto_cpu_dispatch = strdup(config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
    config_valores.puerto_cpu_interrupt = strdup(config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
    config_valores.algoritmo_planificacion = strdup(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
    config_valores.quantum = config_get_int_value(config, "QUANTUM");
    config_valores.recursos = config_get_array_value(config, "RECURSOS");
    config_valores.instancias_recursos = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    config_valores.grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");

}





int iniciar_comunicacion(int argc, char* argv[]) {

    
    logger = log_create("../kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
	log_info(logger, "Se creo el log!");

	//cargar_configuracion("/home/utnso/tp-2024-1c-File-System-Fanatics/kernel/kernel.config");

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

    t_paquete *paquete = crear_paquete(EJECUTAR_PROCESO);

	agregar_a_paquete(paquete, &t_pcb.PID, sizeof(int));
    agregar_a_paquete(paquete, &t_pcb.pc, sizeof(int));

	//Paso el PID a CPU para que identifique el proceso a ejecutar
	
	enviar_paquete(paquete, cpu_dispatch_fd);
	eliminar_paquete(paquete);
}

int leer_grado_multiprogramación() {
    return config_valores.grado_multiprogramacion ;
}


bool permitePasarAREady() {
    return ((leer_grado_multiprogramación()) > (list_size(lista_READY)));
}

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
	server_fd = iniciar_servidor(logger,server_name ,IP, config_valores.puerto_escucha);
	//log_info(logger, "Servidor listo para recibir al cliente");

    printf("Se envio el paquete a cpu y memoria \n");
    while(server_escuchar(server_fd));
	

}

int main(int argc, char *argv[]) {
    

    cargar_configuracion(archivo_configuracion);
    inicializarListas();
    while (1)
    {
        linea = readline(">");

        if (linea)
        {
            add_history(linea);
        }
// PASO 1 - Se ejecutarán los scripts indicados en las pruebas 
        
        if (!strncmp(linea, "EJES ", 5)) // EJECUTAR_SCRIPT
        { // ejecutar script de comandos
            path_script = strdup(linea + strlen("EJECUTAR_SCRIPT "));
            printf("Se esta ejecutando el script del siguiente path: %s\n", path_script);
            
            PCB* recibido = crearPCB();
            printf("Se creo un nuevo proceso: %d\n", recibido->PID);
            planificar_largo_plazo(recibido, path_script);
            printf("Se planifico largo plazo: %d\n", recibido->PID);
            // planificador largo plazo (previamente leyendo el archivo de configuracion con el plani que tiene que utilizar)
            // printf ("Planificando el proceso: %d\n", PID);                
            // cpu el PCB (PID y PC) -- PAQUETES
            // memoria el PCB (Pseudocodigo, PID) -- PAQUETES
            // signal
            // 
            //free(path_script);
            free(linea);
        }

        if (!strncmp(linea, "INICIAR_PROCESO ", 16))
        {
            free(path_proceso);
            path_proceso = strdup(linea + strlen("INICIAR_PROCESO "));

            printf("Se esta ejecutando el proceso: %s\n", path_proceso);
            free(path_proceso);
            free(linea);

            // deberiamos agregar la función de crearPCB en este caso
        }

        if (!strncmp(linea, "FINALIZAR_PROCESO ", 18))
        {
            free(pid);
            pid = strdup(linea + strlen("FINALIZAR_PROCESO "));

            printf("Se finalizo el proceso: %s\n", pid);
            free(pid);
            free(linea);

            // acá habría que poner remove a la lista de procesos en ejecución, para que se libere y pase el siguiente.
        }

        if (!strncmp(linea, "DETENER_PLANIFICACION", 21))
        {
            printf("%s\n", "Se detuvo la planificacion");
            free(linea);
        }

        if (!strncmp(linea, "INICIAR_PLANIFICACION", 21))
        {
            // acá no deberíamos poner un if para validar que la planificación esta detenida?
            printf("%s\n", "Se inicia la planificacion");
            free(linea);
        }

        if (!strncmp(linea, "MULTIPROGRAMACION ", 18))
        {
            free(valor_multiprogramacion);
            valor_multiprogramacion = strdup(linea + strlen("MULTIPROGRAMACION "));

            printf("Se cambio el valor de multiprogramacion a: %s\n", valor_multiprogramacion);
            free(valor_multiprogramacion);
            free(linea);
        }

        if (!strncmp(linea, "PROCESO_ESTADO", 14))
        {
            printf("%s\n", "Se listan los procesos por estado: ");
            free(linea);
        }

        if (!strncmp(linea, "exit", 4))
        {
            free(linea);
            break;
        }

        // printf("%s\n", linea);
        // free(linea);
    }
    return 0;
}
