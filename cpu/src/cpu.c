#include <utils.h>
#include <cicloInstruccion.h>
#include <cpu.h>
#include <signal.h>

pthread_mutex_t mutexSocketKernel = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexSocketCpu = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexInterrupcion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexProcesoEjecutando = PTHREAD_MUTEX_INITIALIZER;
t_list *lista_TLB=NULL;

int tam_pagina;

void paquete_memoria_pedido_tam_pagina(){

    t_paquete *paquete_memoria = crear_paquete(PEDIDO_TAM_PAGINA);

    agregar_entero_a_paquete32(paquete_memoria, 1);

    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);
    
}

void crearTLB(){

    lista_TLB= list_create(); 

}

int pedir_tam_pagina_memoria(){

    int tam_pagina_recibido;

    paquete_memoria_pedido_tam_pagina();
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    
    recv(memoria_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
    recv(memoria_fd, &(paquete->buffer->size), sizeof(int), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(memoria_fd, paquete->buffer->stream, paquete->buffer->size, 0);
    void *stream = paquete->buffer->stream;
    
    switch(paquete->codigo_operacion){
            case ENVIO_TAM_PAGINA:
            {
                memcpy(&tam_pagina_recibido, stream, sizeof(int));
                return tam_pagina_recibido;
            }
            default:
            {   
                log_error(loggerCpu, "Error");
                break;
            }
    }      
    

    return 0;
}
int socketCliente;
void handleSiginitCPU(){
    close(fd_cpu_dispatch);
    close(fd_cpu_interrupt);
    close(memoria_fd);
}

int main(int argc, char* argv[]) {

    signal(SIGINT,handleSiginitCPU);

    iniciarLogger();
    armarConfig();

	//me conecto a memoria
	memoria_fd = crear_conexion(loggerCpu,"MEMORIA",configuracionCpu.IP_MEMORIA, configuracionCpu.PUERTO_MEMORIA);
	log_info(loggerCpu, "Me conecte a memoria!");

    //enviar_mensaje("Hola, soy CPU!", memoria_fd);
	// levanto el servidor dispatch e interrupt
    fd_cpu_interrupt = iniciar_servidor(loggerCpu,server_name_interrupt, configuracionCpu.PUERTO_ESCUCHA_INTERRUPT);
	fd_cpu_dispatch = iniciar_servidor(loggerCpu,server_name_dispatch, configuracionCpu.PUERTO_ESCUCHA_DISPATCH);

	log_info(loggerCpu, "Servidor listo para recibir al cliente");
    
    tam_pagina = pedir_tam_pagina_memoria();

    printf("se recibio tam de pagina :%d\n", tam_pagina);

    crearTLB();
    //Proceso proceso;
    //proceso = recibirProcesoAEjecutar(proceso);

    //pthread_t hiloKernel;
    //pthread_create(&hiloKernel, NULL, atenderPeticionesKernel,NULL);
    pthread_t hiloKernel, hiloEscuchaKernelSocketInterrupt;
    pthread_create(&hiloKernel, NULL, manejarClienteKernel, NULL);
    pthread_create(&hiloEscuchaKernelSocketInterrupt,NULL,check_interrupts,NULL);
    //hilo de ejecucion 
    
    //Hilo de escuchar interrupcion
    pthread_detach(hiloEscuchaKernelSocketInterrupt);
    pthread_join(hiloKernel, NULL);

    while(1){

    }

    return 0;
}


void paquete_memoria_marco(int pid,int pagina){

    t_paquete *paquete_memoria = crear_paquete(SOLICITUD_MARCO);

    agregar_entero_a_paquete32(paquete_memoria, pid);
    agregar_entero_a_paquete32(paquete_memoria, pagina);
    
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);
    
    printf("Se solicita la pagina a memoria: %d\n", pagina);
       
}

void enviar_paquete_mov_in_memoria(int pid, int marco, int desplazamiento){
    t_paquete *paquete_memoria = crear_paquete(MOV_IN);

    agregar_entero_a_paquete32(paquete_memoria, pid);
    agregar_entero_a_paquete32(paquete_memoria, marco);
    agregar_entero_a_paquete32(paquete_memoria, desplazamiento);
    
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);
}


void enviar_paquete_mov_out_memoria( int pid, int marco, int desplazamiento,int size, void* datos){

    t_paquete *paquete_memoria = crear_paquete(MOV_OUT);

    agregar_entero_a_paquete32(paquete_memoria, pid);
    agregar_entero_a_paquete32(paquete_memoria, marco);
    agregar_entero_a_paquete32(paquete_memoria, desplazamiento);
    agregar_a_paquete(paquete_memoria,datos, size);
    
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);
    
}

void paquete_memoria_resize(int PID_paquete,int tam_nuevo){

    t_paquete *paquete_memoria = crear_paquete(RESIZE);

    agregar_entero_a_paquete32(paquete_memoria, PID_paquete);
    agregar_entero_a_paquete32(paquete_memoria, tam_nuevo);
    
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);
    
    printf("Se solicita resize a memoria: %d\n", tam_nuevo);
       
}

void paquete_memoria_pedido_instruccion(int PID_paquete,int PC_paquete){

    t_paquete *paquete_memoria = crear_paquete(PEDIDO_INSTRUCCION);

    agregar_entero_a_paquete32(paquete_memoria, PID_paquete);
    agregar_entero_a_paquete32(paquete_memoria, PC_paquete);
    
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);
    
    printf("se solicita instruccion del pid :%d\n", PID_paquete);
    printf("PC:%d\n", PC_paquete);
    
}


/*void* atenderPeticionesKernel() {
    while (1) {
        int socketCliente = esperarClienteV2(loggerCpu, fd_cpu_dispatch);
        pthread_t client_thread;
        int* pclient = malloc(sizeof(int));
        *pclient = socketCliente;
        pthread_create(&client_thread, NULL, manejarClienteKernel, pclient);
        pthread_detach(client_thread);
    }
    return NULL;
}*/

Proceso *procesoEjecutando = NULL;

void* manejarClienteKernel(void *arg)
{
    //int socketCliente = *((int*)arg);
    socketCliente = esperarClienteV2(loggerCpu, fd_cpu_dispatch);
    //free(arg);
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
                pthread_mutex_lock(&mutexInterrupcion);
                interrumpir = 0;
                pthread_mutex_unlock(&mutexInterrupcion);
                log_info(loggerCpu, "Se recibio el proceso:%d\n", procesoEjecutando->PID);
                pthread_t hiloCicloDeEjecucion;
                pthread_create(&hiloCicloDeEjecucion, NULL, ciclo_de_instruccion,NULL);
                pthread_join(hiloCicloDeEjecucion, NULL);
                break;
            }
            
            default:
            {   
                //log_error(loggerCpu, "Se recibio un operacion de kernel NO valido");
                break;
            }
        }
        eliminar_paquete(paquete);
        //thread_mutex_unlock(&mutexSocketKernel);
    }  
    close(fd_cpu_dispatch);
    close(socketCliente);
}
void* check_interrupts() {
    int socketCliente = esperarClienteV2(loggerCpu, fd_cpu_interrupt);
    while(1){
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

        
        recv(socketCliente, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(socketCliente, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
		void *stream = paquete->buffer->stream;
        int pidInterrumpido;
        switch (paquete->codigo_operacion)
		{
			case PROCESO_INTERRUMPIDO_CLOCK:
			{
				memcpy(&pidInterrumpido, stream, sizeof(uint32_t));
                pthread_mutex_lock(&mutexProcesoEjecutando);
                pthread_mutex_lock(&mutexInterrupcion);
                if(procesoEjecutando->PID == pidInterrumpido){
                    interrumpir = 1;
                }
                pthread_mutex_unlock(&mutexProcesoEjecutando);
                pthread_mutex_unlock(&mutexInterrupcion);
				break;
			}
            case INTERRUMPIR_PROCESO:
            {
                memcpy(&pidInterrumpido, stream, sizeof(uint32_t));
                pthread_mutex_lock(&mutexProcesoEjecutando);
                pthread_mutex_lock(&mutexInterrupcion);
                if(procesoEjecutando->PID == pidInterrumpido){
                    //mutex interrumpir
                    interrumpir = 2;
                }
                pthread_mutex_unlock(&mutexProcesoEjecutando);
                pthread_mutex_unlock(&mutexInterrupcion);
				break;
            }
            default:
            {
                //log_error(loggerCpu,"No es un mensaje correcto para CPU");
                break;
            }
        }
        eliminar_paquete(paquete);
    }
}	