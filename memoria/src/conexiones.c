#include <conexiones.h>
#include <semaforos.h>
#include <math.h>
#include <configs.h>


int server_fd = 0;

//-----------------------------conexion cpu y memoria------------------------------------

int cantidadFrameLibre(){

    int cant_frames_libres = 0;
    int cant_frames_bitarray = bitarray_get_max_bit(memoria.bitmap_frames);
    int diferencia = cant_frames_bitarray-memoria.cantidad_frames;

    for (int i = 0; i < memoria.cantidad_frames; i++) {
    
        int valor = bitarray_test_bit(memoria.bitmap_frames, i);
        
        if(!valor){
            cant_frames_libres ++;
        }
        

    }

    return cant_frames_libres-diferencia;
}

int asignarFrameLibre(){

    for (int i = 0; i < memoria.cantidad_frames; i++) {
    
        int valor = bitarray_test_bit(memoria.bitmap_frames, i);

        if(!valor){

            bitarray_set_bit(memoria.bitmap_frames, i);

            printf("valor nuevo del bitarray: %d\n", bitarray_test_bit(memoria.bitmap_frames, i));
            
            return i;
        }
    
    }

}

void enviar_resultado_instruccion_resize(op_code resultado,int socket_cliente){

    t_paquete *paquete_cpu = crear_paquete(resultado);
    
    enviar_paquete(paquete_cpu, socket_cliente);

    eliminar_paquete(paquete_cpu);
}


op_code actualizar_tam_proceso(int pid_a_cambiar,int tam_a_cambiar){

    Proceso *proceso = NULL;

    for (int i = 0; i < list_size(lista_ProcesosActivos); i++) {

        proceso = list_get(lista_ProcesosActivos,i);

        if (proceso->pid == pid_a_cambiar) {
            
            int cantidad_paginas = ceil(tam_a_cambiar/memoria.pagina_tam);
            int tam_actual = proceso->tam_proceso;

            if (cantidadFrameLibre() == 0 || cantidad_paginas>cantidadFrameLibre()){

                return false;

            }
            else{

            //Caso Ampliacion de un proceso
            if(proceso->tam_proceso == 0){

                proceso->tam_proceso = tam_a_cambiar;
                

                for (int i = 0; i < cantidad_paginas; i++) {

                    Registro_tabla_paginas_proceso *reg_tp_proceso = malloc(sizeof(Registro_tabla_paginas_proceso));
                    
                    reg_tp_proceso->pid_tabla_de_paginas = pid_a_cambiar;
                    reg_tp_proceso->numero_de_pagina = i;
                    
                    reg_tp_proceso->numero_de_frame = asignarFrameLibre();
                    reg_tp_proceso->modificado = false;
                    list_add(proceso->tabla_de_paginas, reg_tp_proceso);

                    i ++;
                }

                return OK;

            }
            
            if(tam_actual<tam_a_cambiar){

                //Agregar paginas
                //int dif_tamaño = tam_actual-tam_a_cambiar;
            

            }

            if(tam_actual>tam_a_cambiar){

                //Remover paginas

            }
            }

        }
    }

}



char* buscar_instruccion(int pid_a_buscar,int pc_a_buscar){

    Proceso *proceso = NULL;

    for (int i = 0; i < list_size(lista_ProcesosActivos); i++) {

        proceso = list_get(lista_ProcesosActivos,i);

        if (proceso->pid == pid_a_buscar) {

            for (int i = 0; i < list_size(proceso->instrucciones); i++) {

                Instruccion *instruccion_ = list_get(proceso->instrucciones, i);

                if (instruccion_->numeroInstruccion == pc_a_buscar) {

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

    agregar_entero_a_paquete32(paquete_cpu, memoria.pagina_tam );
    
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
                paquete_cpu_envio_tam_pagina(socketCliente);
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

            case MOV_OUT:
            {   
                //MOV_OUT (Registro Dirección, Registro Datos): 
                //Lee el valor del Registro Datos y lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
                //int direccion_fisica;
                break;
            }

            case MOV_IN:
            {   
                break;
            }

            case RESIZE:
            {   
                int nuevo_tamaño;
                int pid_a_cambiar;
                op_code resultado_cambio;
                paquete->buffer->stream = malloc(paquete->buffer->size);
                recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
                void *stream = paquete->buffer->stream;

                memcpy(&pid_a_cambiar, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&nuevo_tamaño, stream, sizeof(int));
                printf("Se recibio cambiar el tamanio a: %d\n", nuevo_tamaño );
                resultado_cambio = actualizar_tam_proceso(pid_a_cambiar,nuevo_tamaño);
                enviar_resultado_instruccion_resize(resultado_cambio,socketCliente);                
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
                proceso->tam_proceso = 0;
                proceso->tabla_de_paginas = list_create();
                list_add(lista_ProcesosActivos,proceso);
                printf("se recibio proceso PID:%d\n", proceso->pid);
                printf("se recibio proceso con path:%s\n", proceso->path);
                break;
            }

            case FINALIZAR_PROCESO:
            {   
                //CHECK
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