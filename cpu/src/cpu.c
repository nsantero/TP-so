#include "../include/cpu.h"

//////////////////////////////////////////////////////////////// CONFIGURACIÓN ////////////////////////////////////////////////////////////////

void cargar_configuracion_cpu(char* archivo_configuracion)
{
        t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion

        if (config == NULL) {
                perror("Archivo de configuracion no encontrado");
                exit(-1);
        }

        config_valores.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
        config_valores.puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
        config_valores.puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
        config_valores.puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
        config_valores.cant_enradas_tlb = config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB");
        config_valores.algoritmo_tlb = config_get_string_value(config, "ALGORITMO_TLB");
}

//////////////////////////////////////////////////////////////// FUNCIÓN MAIN ////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	cpu_log = log_create("../cpu.log", "CPU", true, LOG_LEVEL_INFO); // Creación del Log
	log_info(cpu_log, "Se creo el log!");

	char* archivo_config = argv[1]; // Archivo de configuración que llega por path (mediante la consola) 
	cargar_configuracion_cpu(archivo_config);

	socket_memoria = crear_conexion(cpu_log,"CPU",config_valores.ip_memoria, config_valores.puerto_memoria); // Me conecto a memoria 
	log_info(cpu_log, "Me conecte a memoria!");

    enviar_mensaje("Hola, soy CPU!", socket_memoria); // Envio mensaje de prueba 

	fd_cpu_dispatch = iniciar_servidor(cpu_log,server_name_dispatch ,IP, config_valores.puerto_escucha_dispatch); // Levanto el servidor DISPATCH
	fd_cpu_interrupt = iniciar_servidor(cpu_log, server_name_interrupt ,IP, config_valores.puerto_escucha_interrupt); // Levanto el servidor INTERRUPT

	log_info(cpu_log, "Servidor listo para recibir al cliente");
	atender_dispatch(cpu_log); // Escucho al cliente
	
    return EXIT_SUCCESS; // Fin de Proceso? 
}

//////////////////////////////////////////////////////////////// PROCESO CONEXION ////////////////////////////////////////////////////////////////

static void procesar_conexion_dispatch(void* void_args) {
    int *args = (int*) void_args;
        int cliente_socket_dispatch = *args;

    t_identificador cop;
    while (cliente_socket_dispatch != -1 && !recibio_interrupcion) {

        if (recv(cliente_socket_dispatch, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(cpu_log, ANSI_COLOR_BLUE"El cliente se desconecto de DISPATCH");
            return;
        }

        switch (cop) {
            case MENSAJE:
                char* mensaje = recibir_mensaje(cliente_socket_dispatch);
                log_info(cpu_log, "Recibi el mensaje: %s , soy dispatch", mensaje);
                break;

            case PAQUETE:
                t_list *paquete_recibido = recibir_paquete(cliente_socket_dispatch);
                //log_info(cpu_log, ANSI_COLOR_YELLOW "Recibí un paquete con los siguientes valores: ");
                break;

            case ENVIO_PCB: 
                pcb* contexto = recv_pcb(cliente_socket_dispatch);

                if (contexto->pid!=-1) {                    
                    ciclo_instruccion(contexto, cliente_socket_dispatch, cpu_log);
                    destroy_pcb(contexto);
                    break;    
                } else {
                    log_error(cpu_log, "Error al recibir el PCB");
                }
                break;

            default: {
                log_error(cpu_log, "Código de operación no reconocido en Dispatch: %d", cop);
                break;
            }
        }
    }

    return;
}

static void procesar_conexion_interrupt(void* void_args) {
    int *args = (int*) void_args;
        int cliente_socket_interrupt = *args;

    op_code cop;

    while (cliente_socket_interrupt != -1) {

        if (recv(cliente_socket_interrupt, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(cpu_log, ANSI_COLOR_BLUE"El cliente se desconecto de INTERRUPT");
            return;
        }

        switch (cop) {
            case MENSAJE:
                char* mensaje = recibir_mensaje(cliente_socket_interrupt);
                    log_info(cpu_log, "Recibi el mensaje: %s , soy interrupt", mensaje);
                break;
            default: {
                log_error(cpu_log, "Código de operación no reconocido en Interrupt: %d", cop);
                break;
            }
        }
    }    
}

int server_escuchar(int fd_cpu_interrupt, int fd_cpu_dispatch) {
    int socket_cliente_dispatch = esperar_cliente(cpu_log, server_name_dispatch, fd_cpu_dispatch);
    int socket_cliente_interrupt = esperar_cliente(cpu_log, server_name_interrupt, fd_cpu_interrupt);
	
    if (socket_cliente_interrupt != -1 && socket_cliente_dispatch != -1) {

        // Hilo para servidor DISPATCH //
        pthread_t hilo_dispatch;
		pthread_create(&hilo_dispatch, NULL, (void*) procesar_conexion_dispatch, (void*) &socket_cliente_dispatch);
		pthread_detach(hilo_dispatch);

        // Hilo para servidor INTERRUPT
        pthread_t hilo_interrupt;
		pthread_create(&hilo_interrupt, NULL, (void*) procesar_conexion_interrupt, (void*) &socket_cliente_interrupt);
		pthread_detach(hilo_interrupt);

        return 1;
    }
    return 0;
}





	

	
	
