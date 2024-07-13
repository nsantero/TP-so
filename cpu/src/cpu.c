#include <utils.h>
#include <cicloInstruccion.h>
#include <cpu.h>

pthread_mutex_t mutexSocketKernel = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexSocketCpu = PTHREAD_MUTEX_INITIALIZER;

void* escuchar_dispatch(void* arg);

int main(int argc, char* argv[]) {

    iniciarLogger();
    armarConfig();

	//me conecto a memoria
	memoria_fd = crear_conexion(loggerCpu,"CPU",configuracionCpu.IP_MEMORIA, configuracionCpu.PUERTO_MEMORIA);
	log_info(loggerCpu, "Me conecte a memoria!");

    //enviar_mensaje("Hola, soy CPU!", memoria_fd);
	// levanto el servidor dispatch e interrupt
	fd_cpu_dispatch = iniciar_servidor(loggerCpu,server_name_dispatch, configuracionCpu.PUERTO_ESCUCHA_DISPATCH);
    fd_cpu_interrupt = iniciar_servidor(loggerCpu,server_name_interrupt, configuracionCpu.PUERTO_ESCUCHA_INTERRUPT);
	log_info(loggerCpu, "Servidor listo para recibir al cliente");
    
    //Proceso proceso;

    //proceso = recibirProcesoAEjecutar(proceso);

    pthread_t hiloKernel;
    pthread_create(&hiloKernel, NULL, atenderPeticionesKernel,NULL);

	//Hilo de escuchar interrupcion

    //pthread_t hiloEscuchaKernelSocketInterrupt;
    //pthread_create(&hiloEscuchaKernelSocketInterrupt,NULL,escucharInterrupciones,NULL);

    //pthread_join(hiloEscuchaKernelSocketInterrupt,NULL);
    //hilo de ejecucion 
    pthread_detach(hiloKernel); 

    paquete_memoria_pedido_instruccion(1,0);
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    recv(memoria_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
    recv(memoria_fd, &(paquete->buffer->size), sizeof(int), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(memoria_fd, paquete->buffer->stream, paquete->buffer->size, 0);
    void *stream = paquete->buffer->stream;
    switch(paquete->codigo_operacion){
            case ENVIO_INSTRUCCION:
            {
                printf("se recibio instruccion \n");

                break;
            }
            default:
            {   
                log_error(loggerCpu, "Se recibio un operacion de kernel NO valido");
                break;
            }
     }       
    while(1){

    }

    return 0;
}



void paquete_memoria_pedido_instruccion(int PID_paquete,int PC_paquete){

    t_paquete *paquete_memoria = crear_paquete(PEDIDO_INSTRUCCION);

    // Agregar el path al paquete
    agregar_entero_a_paquete32(paquete_memoria, PID_paquete);
    agregar_entero_a_paquete32(paquete_memoria, PC_paquete);
    
    // Pasar PID y txt a memoria
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);
    printf("se envio el pid :%d\n", PID_paquete);
    
}


void* atenderPeticionesKernel() {
    while (1) {
        int socketCliente = esperarClienteV2(loggerCpu, fd_cpu_dispatch);
        pthread_t client_thread;
        int* pclient = malloc(sizeof(int));
        *pclient = socketCliente;
        pthread_create(&client_thread, NULL, manejarClienteKernel, pclient);
        pthread_detach(client_thread);
    }
    return NULL;
}

Proceso *procesoEjecutando = NULL;

void* manejarClienteKernel(void *arg)
{
    int socketCliente = *((int*)arg);
    free(arg);
    while(1){
        //pthread_mutex_lock(&mutexSocketKernel);
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

        recv(socketCliente, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(socketCliente, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
        
    
        switch(paquete->codigo_operacion){
            case EJECUTAR_PROCESO:
            //ejecutar proceso
            {
                procesoEjecutando = malloc(sizeof(Proceso));
                void *stream = paquete->buffer->stream;
                
                memcpy(&procesoEjecutando->PID, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&procesoEjecutando->cpuRegisters.PC, stream, sizeof(int));
                stream += sizeof(uint32_t);
                memcpy(&procesoEjecutando->cpuRegisters.AX, stream, sizeof(uint8_t));
                stream += sizeof(uint8_t);
                memcpy(&procesoEjecutando->cpuRegisters.BX, stream, sizeof(uint8_t));
                stream += sizeof(uint8_t);
                memcpy(&procesoEjecutando->cpuRegisters.CX, stream, sizeof(uint8_t));
                stream += sizeof(uint8_t);
                memcpy(&procesoEjecutando->cpuRegisters.DX, stream, sizeof(uint8_t));
                stream += sizeof(uint8_t);
                memcpy(&procesoEjecutando->cpuRegisters.EAX, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&procesoEjecutando->cpuRegisters.EBX, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&procesoEjecutando->cpuRegisters.ECX, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&procesoEjecutando->cpuRegisters.EDX, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&procesoEjecutando->cpuRegisters.SI, stream, sizeof(uint32_t));
                stream += sizeof(uint32_t);
                memcpy(&procesoEjecutando->cpuRegisters.DI, stream, sizeof(uint32_t));

                printf("se recibio proceso :%d\n", procesoEjecutando->PID);
                //paquete_memoria_pedido_instruccion(procesoEjecutando->PID,procesoEjecutando->cpuRegisters.PC);
            
                break;
            }
            
            default:
            {   
                log_error(loggerCpu, "Se recibio un operacion de kernel NO valido");
                break;
            }
        }
        eliminar_paquete(paquete);
        //thread_mutex_unlock(&mutexSocketKernel);
    }  
    close(fd_cpu_dispatch);
    close(socketCliente);
}
/*
Proceso recibirProcesoAEjecutar(Proceso proceso){

    int socketCliente = esperarClienteV2(logger, fd_cpu_dispatch);
    recv(socketCliente, &(paquete->codigo_operacion), sizeof(op_code), 0);
    recv(socketCliente, &(paquete->buffer->size), sizeof(int), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);

    void *stream = paquete->buffer->stream;

    memcpy(&proceso.PID, stream, sizeof(int));
    stream += sizeof(int);
    memcpy(&proceso.cpuRegisters.PC, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&proceso.cpuRegisters.AX, stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&proceso.cpuRegisters.BX, stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&proceso.cpuRegisters.CX, stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&proceso.cpuRegisters.DX, stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&proceso.cpuRegisters.EAX, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&proceso.cpuRegisters.EBX, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&proceso.cpuRegisters.ECX, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&proceso.cpuRegisters.EDX, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&proceso.cpuRegisters.SI, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&proceso.cpuRegisters.DI, stream, sizeof(uint32_t));

    paquete_memoria_pedido_instruccion(proceso.PID);
    //destruir paquete
    return;
}
*/



/*
void* escuchar_dispatch(void* arg) {
    while (1) {
        int socket_cliente = esperar_cliente(loggerCpu, "CPU Dispatch", fd_cpu_dispatch);
        if (socket_cliente != -1) {
            recibir_proceso(socket_cliente);
        }
    }
    return NULL;
}
*/


/*

static void procesar_conexion_interrupt(void* void_args) {
    int *args = (int*) void_args;
	int cliente_socket_interrupt = *args;

    op_code cop;
	while (cliente_socket_interrupt != -1) {
		if (recv(cliente_socket_interrupt, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
			log_info(logger, ANSI_COLOR_BLUE"El cliente se desconecto de INTERRUPT");
			return;
		}
        switch (cop) {
            case MENSAJE:
                char* mensaje = recibir_mensaje(cliente_socket_interrupt);
				log_info(logger, "Recibi el mensaje: %s , soy interrupt", mensaje);
                break;
            default: {
                log_error(logger, "Código de operación no reconocido en Interrupt: %d", cop);
                break;
            }
        }
    }
}

static void procesar_conexion_dispatch(void* void_args) {
    int *args = (int*) void_args;
	int cliente_socket_dispatch = *args;

    op_code cop;
	while (cliente_socket_dispatch != -1 && !recibio_interrupcion) {
        if (recv(cliente_socket_dispatch, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
			log_info(logger, ANSI_COLOR_BLUE"El cliente se desconecto de DISPATCH");
			return;
		}
        switch (cop) {
            case MENSAJE:
				char* mensaje = recibir_mensaje(cliente_socket_dispatch);
				log_info(logger, "Recibi el mensaje: %s , soy dispatch", mensaje);
                break;
            
            case PEDIDO_INSTRUCCION: {


                break;
            }
		    
            default: {
                log_error(logger, "Código de operación no reconocido en Dispatch: %d", cop);
                break;
            }
        }
    }
    return;
}

int server_escuchar(int fd_cpu_interrupt, int fd_cpu_dispatch) {
	 int socket_cliente_dispatch = esperar_cliente(logger, server_name_dispatch, fd_cpu_dispatch);
	int socket_cliente_interrupt = esperar_cliente(logger, server_name_interrupt, fd_cpu_interrupt);
	
    if (socket_cliente_interrupt != -1 && socket_cliente_dispatch != -1) {
        //hilo para servidor dispatch
        pthread_t hilo_dispatch;
		pthread_create(&hilo_dispatch, NULL, (void*) procesar_conexion_dispatch, (void*) &socket_cliente_dispatch);
		pthread_detach(hilo_dispatch);
        //hilo para servidor interrupt
        pthread_t hilo_interrupt;
		pthread_create(&hilo_interrupt, NULL, (void*) procesar_conexion_interrupt, (void*) &socket_cliente_interrupt);
		pthread_detach(hilo_interrupt);
        return 1;
 	}
    return 0;
}


*/

	

	
	