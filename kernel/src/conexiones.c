#include <conexiones.h>
#include <configs.h>

int memoria_fd;
int cpu_dispatch_fd;
int cpu_interrupt_fd;
int server_fd;
char* server_name = "kernel";

////////////////////////////////////////////////////////// PROCESO CONEXION //////////////////////////////////////////////////////////

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

