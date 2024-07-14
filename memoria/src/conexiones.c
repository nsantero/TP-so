#include <conexiones.h>
#include <semaforos.h>
#include <configs.h>

int server_fd = 0;

//-----------------------------conexion cpu y memoria------------------------------------

char* buscar_instruccion(int pid_a_buscar,int pc_a_buscar){

    Proceso *proceso = NULL;

    for (int i = 0; i < list_size(lista_ProcesosActivos); i++) {

        proceso = list_get(lista_ProcesosActivos,i);
        if (proceso->pid == pid_a_buscar) {

            for (int i = 0; i < list_size(proceso->instrucciones); i++) {
                Instruccion *instruccion_ = list_get(proceso->instrucciones, i);

                if (instruccion_->numeroInstruccion == pc_a_buscar) {
                    //printf("nroinstruccion:%d\n",instruccion_->numeroInstruccion);
                    //printf("instruccion:%s\n",instruccion_->instruccion);
                return instruccion_->instruccion;
                }
            }
        }
    }
}

void paquete_cpu_envio_instruccion(int PID_paquete,int PC_paquete,int socket_cliente){
    t_paquete *paquete_cpu = crear_paquete(ENVIO_INSTRUCCION);

    char* instruccion = buscar_instruccion(PID_paquete,PC_paquete);
    
    agregar_entero_a_paquete32(paquete_cpu, (strlen(instruccion)+1));
    agregar_string_a_paquete(paquete_cpu, instruccion);
    
    enviar_paquete(paquete_cpu, socket_cliente);
    eliminar_paquete(paquete_cpu);

}

void paquete_cpu_envio_tam_pagina(int socket_cliente){

    t_paquete *paquete_cpu = crear_paquete(ENVIO_TAM_PAGINA);

    agregar_entero_a_paquete32(paquete_cpu, memoria.pagina_tam);
    
    enviar_paquete(paquete_cpu, socket_cliente);
    eliminar_paquete(paquete_cpu);

}

void* atenderPeticionesCpu() {
    while (1) {
        int socketCliente = esperarClienteV2(loggerMemoria, server_fd);
        pthread_t client_thread;
        int* pclient = malloc(sizeof(int));
        *pclient = socketCliente;
        pthread_create(&client_thread, NULL, manejarClienteCpu, pclient);
        pthread_detach(client_thread);
    }
    return NULL;
}

void* manejarClienteCpu(void *arg)
{
    int socketCliente = *((int*)arg);
    free(arg);
    while(1){
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));
        recv(socketCliente, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(socketCliente, &(paquete->buffer->size), sizeof(int), 0);
        
        switch(paquete->codigo_operacion){
            case PEDIDO_TAM_PAGINA:
            {   
                //printf("Cpu me pide el tamaño de pagina\n");
                paquete_cpu_envio_tam_pagina(socketCliente);
                //printf("Se envio el tamaño de pagina\n");
                break;
            }
            case PEDIDO_INSTRUCCION:
            {   
                int pid_solicitado;
                int pc_solicitado;
                paquete->buffer->stream = malloc(paquete->buffer->size);
                recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
                void *stream = paquete->buffer->stream;
                memcpy(&pid_solicitado, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&pc_solicitado, stream, sizeof(int));
                printf("pedido de instruccion del pid: %d\n", pid_solicitado );
                printf("pedido de instruccion del pc: %d\n", pc_solicitado );
                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);
                paquete_cpu_envio_instruccion(pid_solicitado,pc_solicitado,socketCliente);
                break;
            }
            default:
            {   
                log_error(loggerMemoria, "Error");
                break;
            }
        }
        eliminar_paquete(paquete);
    }
}


//-----------------------------conexion kernel y memoria------------------------------------
void* atenderPeticionesKernel() {
    while (1) {
        int socketCliente = esperarClienteV2(loggerMemoria, server_fd);
        pthread_t client_thread;
        int* pclient = malloc(sizeof(int));
        *pclient = socketCliente;
        pthread_create(&client_thread, NULL, manejarClienteKernel, pclient);
        pthread_detach(client_thread);
    }
    return NULL;
}

void* manejarClienteKernel(void *arg)
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
        
        switch(paquete->codigo_operacion){
            case CREAR_PROCESO:
            {
                Proceso *proceso = malloc(sizeof(Proceso));

                void *stream = paquete->buffer->stream;
                int pathLenght;

                memcpy(&proceso->pid, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&pathLenght, stream, sizeof(int));
                stream += sizeof(int);
                proceso->path = malloc(pathLenght);
                memcpy(proceso->path, stream, pathLenght);
                cargarInstrucciones(proceso, proceso->path);
                list_add(lista_ProcesosActivos,proceso);
                printf("se recibio proceso PID:%d\n", proceso->pid);
                printf("se recibio proceso con path:%s\n", proceso->path);
                break;
            }
            case FINALIZAR_PROCESO:
            {   
                int pid_remover;
                void *stream = paquete->buffer->stream;
                memcpy(&pid_remover, stream, sizeof(int));
                printf("finalizar proceso:%d\n", pid_remover);
                list_remove_element(lista_ProcesosActivos, &pid_remover);
                break;
            }
            
            case IO_FS_READ:
            {
                //RECIBE DIRECCION y BUFFER y escribe el buffer en la direccion dada
            }
            case  IO_FS_WRITE:
            {
                //RECIBE DIRECCION , TAM y luego envia lo que hay en esa direccion a IOFS
            }
            case  IO_STDIN_READ :
            {
                //RECIBE TAMAÑO DIRECCION Y UN STRING, LUEGO DEBE GUARDARSE EN DIRECCION
            }
            case  IO_STDOUT_WRITE :
            {
                //REBIBE TAMAÑO Y DIRECCION, DEBE LEER LA DIRECCION Y MANDAR EL CONTINIDO 
            }
            default:
            {   
                log_error(loggerMemoria, "Se recibio un operacion de kernel NO valido");
                break;
            }
        }
        eliminar_paquete(paquete);
    }

    close(socketCliente);
}

void cargarInstrucciones(Proceso *proceso, const char *path) {
    int valorInstruccion = 0;
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }
    
    proceso->instrucciones = list_create();

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;

        Instruccion *instruccion = malloc(sizeof(Instruccion));
        instruccion->instruccion = strdup(line);
        instruccion->numeroInstruccion = valorInstruccion;
        //printf("INSTRUCCION NRO:%d\n", instruccion->numeroInstruccion);
        //printf("INSTRUCCION:%s\n", instruccion->instruccion);
        list_add(proceso->instrucciones, instruccion);
        valorInstruccion ++;
    }

    fclose(file);
}