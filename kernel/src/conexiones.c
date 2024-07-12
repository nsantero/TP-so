#include <conexiones.h>
#include <configs.h>

int memoria_fd;
int cpu_dispatch_fd=0;
int cpu_interrupt_fd=0;
int server_fd;
char* server_name = "kernel";
int socketCliente;

t_list *interfacesConectadas;

////////////////////////////////////////////////////////// PROCESO CONEXION //////////////////////////////////////////////////////////

void* conexionesDispatch()
{
	
	while (1)
	{

		t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

        recv(cpu_dispatch_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(cpu_dispatch_fd, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(cpu_dispatch_fd, paquete->buffer->stream, paquete->buffer->size, 0);
		void *stream = paquete->buffer->stream;
		switch (paquete->codigo_operacion)
		{
			case PROCESO_EXIT:
			{
				//mutex listas	
				PCB* proceso = cambiarAExitDesdeRunning(lista_RUNNING);
				//mutex conexion memoria
				paquete_memoria_finalizar_proceso(proceso->PID);

				log_info(loggerKernel, "Se elimino el proceso con pid: %d\n", proceso->PID);

				//liberar proceso Kernel
				//eliminarProceso(proceso); //TODO
				break;
			}
			case PROCESO_INTERRUMPIDO:
			{
				//mutex lista
				//en caso de ser RR enviar a lista de ready al final
				//en caso de ser VRR 
				break;
			}
			case PROCESO_WAIT:
			{
				
				break;
			}
			case PROCESO_SIGNAL:
			{
				break;
			}
			case IO_GEN_SLEEP:
			{
				Peticion_Interfaz_Generica interfazGenerica;

				PCB* proceso;

				int pathLength;

				memcpy(&interfazGenerica.unidades_de_trabajo, stream, sizeof(int));
				stream += sizeof(int);
				memcpy(&interfazGenerica.PID, stream, sizeof(int));
				stream += sizeof(int);
				memcpy(&pathLength, stream, sizeof(int));
				interfazGenerica.nombre_interfaz = malloc(pathLength);
				memcpy(interfazGenerica.nombre_interfaz, stream, pathLength);

				//MUTEX
				if(existeInterfaz(interfazGenerica.nombre_interfaz)){
					enviar_paquete(paquete, socketCliente);
				}
				else{
					PCB* proceso = cambiarAExitDesdeRunning(lista_RUNNING);
					paquete_memoria_finalizar_proceso(proceso->PID);
					//eliminarProceso(proceso); //TODO
				}
				
				break;
			}
				
			default:
			{
				//ningun mensaje valido recibido
				break;
			}
			eliminar_paquete(paquete);
		}
	}
}

int existeInterfaz(char *nombre){
	//mutex lista
	Interfaces_conectadas_kernel *interfazBuffer;
	for (int i = 0; i < list_size(interfacesConectadas); i++)
	{
		interfazBuffer=list_get(interfacesConectadas, i);
		if(!strcmp(interfazBuffer->nombre, nombre)){
			return 1;
		}

	}
	return 0;
}

void* atenderPeticionesIO() {
    while (1) {
        int socketCliente = esperarClienteV2(loggerKernel, server_fd);
        pthread_t client_thread;
        int* pclient = malloc(sizeof(int));
        *pclient = socketCliente;
        pthread_create(&client_thread, NULL, manejarClienteIO, pclient);
        pthread_detach(client_thread);
    }
    return NULL;
}

void* manejarClienteIO(void *arg)
{
    int socketCliente = *((int*)arg);
    free(arg);
    while(1){
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

        
        recv(socketCliente, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(socketCliente, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
		void *stream = paquete->buffer->stream;
		switch(paquete->codigo_operacion){
        	case AGREGAR_INTERFACES:
            {
				Interfaces_conectadas_kernel *interfazBuffer;
				int cantidadDeInterfaces;
				int charTam;
				memcpy(&charTam, stream, sizeof(int));
				stream += sizeof(int);
				interfazBuffer->nombre = malloc(charTam);
				memcpy(&interfazBuffer->nombre,stream, sizeof(charTam));
				stream += charTam;
				memcpy(&interfazBuffer->tipo, stream , sizeof(Tipos_Interfaz));

				interfazBuffer->ocupada = 0;
				interfazBuffer->solicitudesEnCola = 0;
				interfazBuffer->pidActual = -1;

				list_add(interfacesConectadas, interfazBuffer);

				log_info(loggerKernel, "Se conecto y guardo la interfaz con nombre:%s",interfazBuffer->nombre);
				
				break;
			}
			case TERMINO_INTERFAZ:
			{
				//recibo nombre de la interfaz para sacarlo de la lista y pasarlo de bloqueado a ready
			}
			default:
			{
				//ningun mensaje valido recibido
				break;
			}
		}
	}
}



/*static void procesar_conexion(void *void_args) {
	int *args = (int*) void_args;
	int cliente_socket = *args;
	op_code cop;

	while (cliente_socket != -1) {
		if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
			log_info(loggerKernel, ANSI_COLOR_BLUE"El cliente se desconecto de %s server", server_name);
			return;
		}
		switch (cop) {
		case MENSAJE:{

			char* mensaje = recibir_mensaje(cliente_socket);
            log_info(loggerKernel, ANSI_COLOR_YELLOW"Me llegó el mensaje: %s", mensaje);

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
}*/

/*int server_escuchar(int fd_memoria) {

	int cliente_socket = esperar_cliente(loggerKernel, server_name, fd_memoria);

	if (cliente_socket != -1) {
		pthread_t hilo;
		pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) &cliente_socket);
		pthread_detach(hilo);
		return 1;
	}

	return 0;
}*/

