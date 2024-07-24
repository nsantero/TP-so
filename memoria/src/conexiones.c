#include <conexiones.h>
#include <semaforos.h>
#include <math.h>
#include <configs.h>


int server_fd = 0;

//-----------------------------conexion cpu y memoria------------------------------------

void liberarFrame(int frame){

    bitarray_clean_bit(memoria.bitmap_frames, frame);

    log_info(loggerMemoria, "Se liberó el frame:%d", frame);

}

void destroy_page_entry(void *element) {

    Registro_tabla_paginas_proceso *reg_tp_proceso = (Registro_tabla_paginas_proceso *)element;
    free(reg_tp_proceso); 
}

void destroy_process_entry(void *element) {

    Proceso *proceso = (Proceso *)element;
    free(proceso); 
}

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

    log_info(loggerMemoria, "Cantidad de frames libres:%d",cant_frames_libres-diferencia);
    return cant_frames_libres-diferencia;
}

int asignarFrameLibre(){

    for (int i = 0; i < memoria.cantidad_frames; i++) {
    
        int valor = bitarray_test_bit(memoria.bitmap_frames, i);

        if(!valor){

            bitarray_set_bit(memoria.bitmap_frames, i);

            log_info(loggerMemoria, "Se asigno el frame:%d",i);
            log_info(loggerMemoria, "El frame esta en :%d",bitarray_test_bit(memoria.bitmap_frames, i));
            
            return i;
        }
    
    }

}

void enviar_resultado_instruccion_resize(op_code resultado,int socket_cliente){

    t_paquete *paquete_cpu = crear_paquete(resultado);
    
    enviar_paquete(paquete_cpu, socket_cliente);

    log_info(loggerMemoria, "Se envio a CPU el resultado del resize.");

    eliminar_paquete(paquete_cpu);
}

void enviar_paquete_cpu_mov_out(op_code resultado,int socketCliente){
   
    t_paquete *paquete_cpu = crear_paquete(resultado);
    
    enviar_paquete(paquete_cpu, socketCliente);

    log_info(loggerMemoria, "Se envio a CPU el resultado del mov out.");

    eliminar_paquete(paquete_cpu);
}


op_code actualizar_tam_proceso(int pid_a_cambiar,int tam_a_cambiar){

    Proceso *proceso = NULL;

    for (int i = 0; i < list_size(lista_ProcesosActivos); i++) {

        proceso = list_get(lista_ProcesosActivos,i);

        if (proceso->pid == pid_a_cambiar) {
            
            //int cantidad_paginas = ceil(tam_a_cambiar/memoria.pagina_tam);

            double calculo_paginas = (double)tam_a_cambiar/memoria.pagina_tam;

            int cantidad_paginas = round(calculo_paginas);
            
            log_info(loggerMemoria, "Cantidad de paginas del nuevo tamaño:%d", cantidad_paginas);

            int tam_actual = proceso->tam_proceso;
            
            log_info(loggerMemoria, "Tamaño actual del proceso:%d", tam_actual);

            if (cantidadFrameLibre() == 0 || cantidad_paginas>cantidadFrameLibre()){

                return OUT_OF_MEMORY;

            }

            else{

                //Caso Ampliacion de un proceso
                if(proceso->tam_proceso == 0){

                    log_info(loggerMemoria, "Se amplia el proceso.");

                    for (int i = 0; i < cantidad_paginas; i++) {

                        Registro_tabla_paginas_proceso *reg_tp_proceso = malloc(sizeof(Registro_tabla_paginas_proceso));
                        
                        reg_tp_proceso->pid_tabla_de_paginas = pid_a_cambiar;
                        reg_tp_proceso->numero_de_pagina = i;
                        log_info(loggerMemoria, "Se agrego la pagina: %d",reg_tp_proceso->numero_de_pagina);
                        reg_tp_proceso->numero_de_frame = asignarFrameLibre();
                        reg_tp_proceso->modificado = false;

                        log_info(loggerMemoria, "Modificado: %s",reg_tp_proceso->modificado);

                        list_add(proceso->tabla_de_paginas, reg_tp_proceso);

                    }

                    proceso->tam_proceso = tam_a_cambiar;
                    proceso->cantidad_paginas_asiganadas = cantidad_paginas;
                    log_info(loggerMemoria, "Se amplio el proceso.");

                    return OK;

                }
            
                if(tam_actual != 0 && tam_actual<tam_a_cambiar){

                    //Agregar paginas
                    int dif_cantidad = cantidad_paginas- proceso->cantidad_paginas_asiganadas;

                    log_info(loggerMemoria, "Se amplia el proceso con la siguiente cantidad de paginas: %d",dif_cantidad);

                    for (int i = 0; i < dif_cantidad; i++) {

                        Registro_tabla_paginas_proceso *reg_tp_proceso = malloc(sizeof(Registro_tabla_paginas_proceso));
                        
                        reg_tp_proceso->pid_tabla_de_paginas = pid_a_cambiar;
                        reg_tp_proceso->numero_de_pagina = proceso->cantidad_paginas_asiganadas+i;
                        log_info(loggerMemoria, "Se agrego la pagina: %d",reg_tp_proceso->numero_de_pagina);
                        reg_tp_proceso->numero_de_frame = asignarFrameLibre();
                        reg_tp_proceso->modificado = false;
                        log_info(loggerMemoria, "Modificado: %s",reg_tp_proceso->modificado);

                        
                        list_add(proceso->tabla_de_paginas, reg_tp_proceso);
                        

                    }
                    proceso->tam_proceso = tam_a_cambiar;
                    proceso->cantidad_paginas_asiganadas = cantidad_paginas;
                    log_info(loggerMemoria, "Se amplio el proceso.");
                    cantidadFrameLibre();
                    return OK;
                
                }

                if(tam_actual != 0 && tam_actual>tam_a_cambiar){

                    //Remover paginas
                    
                    int dif_cantidad = proceso->cantidad_paginas_asiganadas-cantidad_paginas;

                    log_info(loggerMemoria, "Se reduce el proceso con la siguiente cantidad de paginas: %d",dif_cantidad);
                    // cant pag actuales 4
                    // cant pag a cambiar 3
                    // dif 1

                    // for i= 3; i== 2;i--
                    int size = list_size(proceso->tabla_de_paginas)-1; //3
                    int hasta = list_size(proceso->tabla_de_paginas)-dif_cantidad; //1

                    for (int i = size; i >= hasta; i--) {

                        Registro_tabla_paginas_proceso *reg_tp_proceso = list_get(proceso->tabla_de_paginas,i);

                        log_info(loggerMemoria, "Se elimina la pagina:%d", reg_tp_proceso->numero_de_pagina);    
                    
                        liberarFrame(reg_tp_proceso->numero_de_frame); 
                        log_info(loggerMemoria, "Modificado: %s",reg_tp_proceso->modificado);
                         
                        list_remove_and_destroy_element(proceso->tabla_de_paginas, i,destroy_page_entry);
                        
                    }
                    proceso->tam_proceso = tam_a_cambiar;
                    proceso->cantidad_paginas_asiganadas = cantidad_paginas;
                    log_info(loggerMemoria, "cantidad actual de paginas:%d",list_size(proceso->tabla_de_paginas));    
                    log_info(loggerMemoria, "Se reducio el proceso.");
                
                    cantidadFrameLibre();
                    return OK;

                }

            }

        }
    }

}





char* buscar_instruccion(int pid_a_buscar,int pc_a_buscar){

    Proceso *proceso = NULL;
    Instruccion *instruccionBuscada = NULL;

    for (int i = 0; i < list_size(lista_ProcesosActivos); i++) {

        proceso = list_get(lista_ProcesosActivos,i);

        if (proceso->pid == pid_a_buscar) {

            for (int i = 0; i < list_size(proceso->instrucciones); i++) {

               instruccionBuscada = list_get(proceso->instrucciones, i);

                if (instruccionBuscada->numeroInstruccion == pc_a_buscar) {

                    return instruccionBuscada->instruccion;

                }
            }
        }
    }
    return NULL;
}

void enviar_paquete_cpu_marco(int marco_encontrado,int socketCliente){

    t_paquete *paquete_cpu = crear_paquete(ENVIO_MARCO);

    agregar_entero_a_paquete32(paquete_cpu, marco_encontrado );
    
    enviar_paquete(paquete_cpu, socketCliente);

    log_info(loggerMemoria, "Se envio a CPU el marco: %d",marco_encontrado);

    eliminar_paquete(paquete_cpu);


}

void paquete_cpu_envio_instruccion(int PID_paquete,int PC_paquete,int socket_cliente){

    t_paquete *paquete_cpu = crear_paquete(ENVIO_INSTRUCCION);

    char* instruccion = buscar_instruccion(PID_paquete,PC_paquete);
    
    agregar_entero_a_paquete32(paquete_cpu, (strlen(instruccion)+1));
    agregar_string_a_paquete(paquete_cpu, instruccion);
    
    enviar_paquete(paquete_cpu, socket_cliente);

    log_info(loggerMemoria, "Se envio a CPU la instruccion: %s",instruccion);

    eliminar_paquete(paquete_cpu);

}

void paquete_cpu_envio_tam_pagina(int socket_cliente){

    t_paquete *paquete_cpu = crear_paquete(ENVIO_TAM_PAGINA);

    agregar_entero_a_paquete32(paquete_cpu, memoria.pagina_tam );
    
    enviar_paquete(paquete_cpu, socket_cliente);

    log_info(loggerMemoria, "Se envio el tamaño de pagina a CPU: %d",memoria.pagina_tam);

    eliminar_paquete(paquete_cpu);

}

void remover_proceso(int pid_remover){

    Proceso *proceso = NULL;

    for (int i = 0; i < list_size(lista_ProcesosActivos); i++) {

        proceso = list_get(lista_ProcesosActivos,i);

        if (proceso->pid == pid_remover) {

            //liberar frames

            for (int i = (proceso->cantidad_paginas_asiganadas-1); i >=0; i--) {
                
                Registro_tabla_paginas_proceso *reg_tp_proceso = list_get(proceso->tabla_de_paginas,i);
                
                log_info(loggerMemoria, "Se elimina la pagina:%d", reg_tp_proceso->numero_de_pagina);
                int frame_liberar = reg_tp_proceso->numero_de_frame;
                liberarFrame(frame_liberar); 
                list_remove_and_destroy_element(proceso->tabla_de_paginas, i,destroy_page_entry);

            }
            
            list_remove_and_destroy_element(lista_ProcesosActivos, i,destroy_process_entry);
            cantidadFrameLibre();
            
            break;

        }
    }
}

Proceso* obtener_proceso(int pid_remover){

    Proceso *proceso = NULL;

    for (int i = 0; i < list_size(lista_ProcesosActivos); i++) {

        proceso = list_get(lista_ProcesosActivos,i);

        if (proceso->pid == pid_remover) {

            return proceso;

        }
    }
}

void enviar_paquete_cpu_mov_in(uint32_t datos,int socketCliente){

    t_paquete *paquete_cpu = crear_paquete(ENVIO_MOV_IN);

    agregar_entero_a_paquete32(paquete_cpu, datos);
    
    enviar_paquete(paquete_cpu, socketCliente);

    log_info(loggerMemoria, "Se envio a CPU el MOV_IN: %d",datos);

    eliminar_paquete(paquete_cpu);


}

int obtener_marco(int pid,int pagina){

    Proceso *proceso = NULL;

    for (int i = 0; i < list_size(lista_ProcesosActivos); i++) {

        proceso = list_get(lista_ProcesosActivos,i);

        if (proceso->pid == pid) {

            for (int i = 0; i < list_size(proceso->tabla_de_paginas); i++) {

                Registro_tabla_paginas_proceso * reg_tabla_pag = list_get(proceso->tabla_de_paginas, i);

                if (reg_tabla_pag->pid_tabla_de_paginas == pid && reg_tabla_pag->numero_de_pagina == pagina) {

                    return reg_tabla_pag->numero_de_frame;

                }
            }
        }
    }


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
                log_info(loggerMemoria, "CPU solicito el tamaño de pagina. \n");
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
                log_info(loggerMemoria, "Se solicita la intruccion del PID:%d", pid_solicitado);
                log_info(loggerMemoria, "Se solicita el PC:%d", pc_solicitado);
                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);
                paquete_cpu_envio_instruccion(pid_solicitado,pc_solicitado,socketCliente);
                break;
            }

            case MOV_OUT:
            {   
                int pid_mov_out; // es necesario????????
                int marco_mov_out;
                int desplazamiento_mov_out;
                uint32_t datos;
                Proceso *proceso = malloc(sizeof(Proceso));

                paquete->buffer->stream = malloc(paquete->buffer->size);
                recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
                void *stream = paquete->buffer->stream;

                memcpy(&pid_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&marco_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&desplazamiento_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                //proceso = obtener_proceso(pid_mov_out);

                //(void* valor, uint32_t tamanio, uint32_t direccion_fisica).
                //Escribir en memoria
                memcpy((char*)memoria.espacioUsuario+(marco_mov_out*memoria.pagina_tam)+desplazamiento_mov_out, stream, sizeof(uint32_t));

                // Leer el valor de la memoria :)
                uint32_t valor;
                char* ptr = (char*)memoria.espacioUsuario+(marco_mov_out*memoria.pagina_tam)+desplazamiento_mov_out;
                memcpy(&valor, ptr, sizeof(uint32_t));
                log_info(loggerMemoria, "valor escrito:%d", valor);
                //

                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);
                enviar_paquete_cpu_mov_out(OK,socketCliente);

                break;
            }

            case MOV_IN:
            {   
                int pid_mov_out;
                int marco_mov_out;
                int desplazamiento_mov_out;
                uint32_t datos;
                Proceso *proceso = malloc(sizeof(Proceso));

                paquete->buffer->stream = malloc(paquete->buffer->size);
                recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
                void *stream = paquete->buffer->stream;

                memcpy(&pid_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&marco_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&desplazamiento_mov_out, stream, sizeof(int));
                stream += sizeof(int);


                char* direccion = (char*)memoria.espacioUsuario+(marco_mov_out*memoria.pagina_tam)+desplazamiento_mov_out;
                memcpy(&datos, direccion, sizeof(uint32_t));

                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);

                enviar_paquete_cpu_mov_in(datos,socketCliente);
               
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
                log_info(loggerMemoria, "Se solicita resize del PID:%d", pid_a_cambiar);
                log_info(loggerMemoria, "Se solicita el tamaño:%d", nuevo_tamaño);
                resultado_cambio = actualizar_tam_proceso(pid_a_cambiar,nuevo_tamaño);
                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);
                enviar_resultado_instruccion_resize(resultado_cambio,socketCliente);                
                break;
            }
            case SOLICITUD_MARCO:
            {   
                int pid_solicitado;
                int pagina_solicitada;

                paquete->buffer->stream = malloc(paquete->buffer->size);
                recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
                void *stream = paquete->buffer->stream;

                memcpy(&pid_solicitado, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&pagina_solicitada, stream, sizeof(int));

                log_info(loggerMemoria, "Se solicita la pagina del PID:%d", pid_solicitado);
                log_info(loggerMemoria, "Se solicita la pagina:%d", pagina_solicitada);

                int marco_encontrado;
                marco_encontrado= obtener_marco(pid_solicitado,pagina_solicitada);
                enviar_paquete_cpu_marco(marco_encontrado,socketCliente);
                break;
            }
            default:
            {   
                //log_error(loggerMemoria, "Error");
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
        void *stream = paquete->buffer->stream;
        
        switch(paquete->codigo_operacion){

            case CREAR_PROCESO:
            {
                Proceso *proceso = malloc(sizeof(Proceso));

                
                int pathLenght;

                memcpy(&proceso->pid, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&pathLenght, stream, sizeof(int));
                stream += sizeof(int);
                proceso->path = malloc(pathLenght);
                memcpy(proceso->path, stream, pathLenght);
                log_info(loggerMemoria, "Se creo el proceso con el PID:%d", proceso->pid);
                log_info(loggerMemoria, "Se creo el proceso con el path:%s", proceso->path);
                cargarInstrucciones(proceso, proceso->path);
                log_info(loggerMemoria, "Se cargaron las instrucciones");
                proceso->tam_proceso = 0;
                proceso->tabla_de_paginas = list_create();
                list_add(lista_ProcesosActivos,proceso);
                break;
            }
            case FINALIZAR_PROCESO:
            {   
                //CHECK
                int pid_remover;
                memcpy(&pid_remover, stream, sizeof(int));
                remover_proceso(pid_remover);
                log_info(loggerMemoria, "Se elimino el proceso:%d", pid_remover);
                break;
            }
            //CPU
            case PEDIDO_TAM_PAGINA:
            {   
                log_info(loggerMemoria, "CPU solicito el tamaño de pagina. \n");
                paquete_cpu_envio_tam_pagina(socketCliente);
                break;
            }

            case PEDIDO_INSTRUCCION:
            {   
                int pid_solicitado;
                int pc_solicitado;
                memcpy(&pid_solicitado, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&pc_solicitado, stream, sizeof(int));
                log_info(loggerMemoria, "Se solicita la intruccion del PID:%d", pid_solicitado);
                log_info(loggerMemoria, "Se solicita el PC:%d", pc_solicitado);
                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);
                paquete_cpu_envio_instruccion(pid_solicitado,pc_solicitado,socketCliente);
                break;
            }

            case MOV_OUT:
            {   
                int pid_mov_out; // es necesario????????
                int marco_mov_out;
                int desplazamiento_mov_out;
                uint32_t datos;
                Proceso *proceso = malloc(sizeof(Proceso));

                memcpy(&pid_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&marco_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&desplazamiento_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                //proceso = obtener_proceso(pid_mov_out);

                //(void* valor, uint32_t tamanio, uint32_t direccion_fisica).
                //Escribir en memoria
                memcpy((char*)memoria.espacioUsuario+(marco_mov_out*memoria.pagina_tam)+desplazamiento_mov_out, stream, sizeof(uint32_t));

                // Leer el valor de la memoria :)
                uint32_t valor;
                char* ptr = (char*)memoria.espacioUsuario+(marco_mov_out*memoria.pagina_tam)+desplazamiento_mov_out;
                memcpy(&valor, ptr, sizeof(uint32_t));
                log_info(loggerMemoria, "valor escrito:%d", valor);
                //

                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);
                enviar_paquete_cpu_mov_out(OK,socketCliente);

                break;
            }

            case MOV_IN:
            {   
                int pid_mov_out;
                int marco_mov_out;
                int desplazamiento_mov_out;
                uint32_t datos;
                Proceso *proceso = malloc(sizeof(Proceso));

                memcpy(&pid_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&marco_mov_out, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&desplazamiento_mov_out, stream, sizeof(int));
                stream += sizeof(int);


                char* direccion = (char*)memoria.espacioUsuario+(marco_mov_out*memoria.pagina_tam)+desplazamiento_mov_out;
                memcpy(&datos, direccion, sizeof(uint32_t));

                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);

                enviar_paquete_cpu_mov_in(datos,socketCliente);
               
                break;
            }

            case RESIZE:
            {   
                int nuevo_tamaño;
                int pid_a_cambiar;

                op_code resultado_cambio;

                memcpy(&pid_a_cambiar, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&nuevo_tamaño, stream, sizeof(int));
                log_info(loggerMemoria, "Se solicita resize del PID:%d", pid_a_cambiar);
                log_info(loggerMemoria, "Se solicita el tamaño:%d", nuevo_tamaño);
                resultado_cambio = actualizar_tam_proceso(pid_a_cambiar,nuevo_tamaño);
                usleep(configuracionMemoria.RETARDO_RESPUESTA*1000);
                enviar_resultado_instruccion_resize(resultado_cambio,socketCliente);                
                break;
            }
            case SOLICITUD_MARCO:
            {   
                int pid_solicitado;
                int pagina_solicitada;

                memcpy(&pid_solicitado, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&pagina_solicitada, stream, sizeof(int));

                log_info(loggerMemoria, "Se solicita la pagina del PID:%d", pid_solicitado);
                log_info(loggerMemoria, "Se solicita la pagina:%d", pagina_solicitada);

                int marco_encontrado;
                marco_encontrado= obtener_marco(pid_solicitado,pagina_solicitada);
                enviar_paquete_cpu_marco(marco_encontrado,socketCliente);
                break;
            }
            case IO_MEM_FS_READ:
            {
                uint32_t tamanio;
                uint32_t direc;
                void* buffer;
                memcpy(&tamanio,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                memcpy(&direc,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                buffer=malloc(tamanio);
                memcpy(buffer,stream,tamanio);
                
                //Guardar buffer

                free(buffer);
                break;
            }
            case  IO_MEM_FS_WRITE:
            {
                uint32_t tamanio;
                uint32_t direc;
                memcpy(&tamanio,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                memcpy(&direc,stream,sizeof(uint32_t));
                
                //Lee la direccion
                char* hardcodeo="Esta prueba esta hardcodeada";
                void* bufferEncontrado=hardcodeo;

                t_paquete* paqueteDevolverAIO=crear_paquete(IO_MEM_FS_WRITE);
                agregar_a_paquete(paqueteDevolverAIO,bufferEncontrado,tamanio);
                enviar_paquete(paqueteDevolverAIO,socketCliente);
                free(paqueteDevolverAIO->buffer);
                free(paqueteDevolverAIO);
                

                break;

            }
            case  IO_MEM_STDIN_READ :
            {
                uint32_t tamanio;
                uint32_t direccion;
                void* buffer;
                memcpy(&tamanio,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                memcpy(&direccion,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                buffer=malloc(tamanio);
                memcpy(buffer,stream,sizeof(tamanio));

                //TODO aca tiene q escribir el buffer en la direccion q le manda IO
                // solicitar la traduccion de dl a df
                //Devolver ok?
                free(buffer);
                break;
            }
            case  IO_MEM_STDOUT_WRITE :
            {
                //REBIBE TAMAÑO Y DIRECCION, DEBE LEER LA DIRECCION Y MANDAR EL CONTINIDO 
                uint32_t tamanio;
                uint32_t direccion;
                memcpy(&tamanio,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                memcpy(&direccion,stream,sizeof(uint32_t));

                //TODO busca la direccion y la mete en el buffer;
                char* hardcodeo="Esta prueba esta hardcodeada";
                void* bufferEncontrado=hardcodeo;

                t_paquete* paqueteDevolverAIO=crear_paquete(IO_MEM_STDOUT_WRITE);
                agregar_a_paquete(paqueteDevolverAIO,bufferEncontrado,tamanio);
                enviar_paquete(paqueteDevolverAIO,socketCliente);
                free(paqueteDevolverAIO->buffer);
                free(paqueteDevolverAIO);
                break;
                
            }
            default:
            {   
                //log_error(loggerMemoria, "Se recibio un operacion de kernel NO valido");
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
        //TODO ESTO ACA HABRIA Q LOGGEARLO O Q ESCRIBA POR PANTALLA O ALGO
        //ME ESTABA TIRANDO SEGMENTATION FAULT POR Q ESTABA MAL EL PATH Y NUNCA ME ENTERE
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

//-----------------------------conexion EntradaSalida------------------------------------

void* atenderPeticionesEntradaSalida() {

    while (1) {
        int socketCliente = esperarClienteV2(loggerMemoria, server_fd);
        pthread_t client_thread;
        int* pclient = malloc(sizeof(int));
        *pclient = socketCliente;
        pthread_create(&client_thread, NULL, manejarClienteEntradaSalida, pclient);
        pthread_detach(client_thread);
    }

    return NULL;
}

void* manejarClienteEntradaSalida(void *arg)
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

            case IO_MEM_FS_READ:
            {
                uint32_t tamanio;
                uint32_t direc;
                void* buffer;
                memcpy(&tamanio,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                memcpy(&direc,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                buffer=malloc(tamanio);
                memcpy(buffer,stream,tamanio);
                
                //Guardar buffer



                free(buffer);
                break;
            }
            case  IO_MEM_FS_WRITE:
            {
                uint32_t tamanio;
                uint32_t direc;
                memcpy(&tamanio,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                memcpy(&direc,stream,sizeof(uint32_t));
                
                //Lee la direccion
                char* hardcodeo="Esta prueba esta hardcodeada";
                void* bufferEncontrado=hardcodeo;

                t_paquete* paqueteDevolverAIO=crear_paquete(IO_MEM_FS_WRITE);
                agregar_a_paquete(paqueteDevolverAIO,bufferEncontrado,tamanio);
                enviar_paquete(paqueteDevolverAIO,socketCliente);
                free(paqueteDevolverAIO->buffer);
                free(paqueteDevolverAIO);
                

                break;

            }
            case  IO_MEM_STDIN_READ :
            {
                uint32_t tamanio;
                uint32_t direccion;
                void* buffer;
                memcpy(&tamanio,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                memcpy(&direccion,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                buffer=malloc(tamanio);
                memcpy(buffer,stream,sizeof(tamanio));

                //TODO aca tiene q escribir el buffer en la direccion q le manda IO
                // solicitar la traduccion de dl a df
                //Devolver ok?
                free(buffer);
                break;
            }
            case  IO_MEM_STDOUT_WRITE :
            {
                //REBIBE TAMAÑO Y DIRECCION, DEBE LEER LA DIRECCION Y MANDAR EL CONTINIDO 
                uint32_t tamanio;
                uint32_t direccion;
                memcpy(&tamanio,stream,sizeof(uint32_t));
                stream+=sizeof(uint32_t);
                memcpy(&direccion,stream,sizeof(uint32_t));

                //TODO busca la direccion y la mete en el buffer;
                char* hardcodeo="Esta prueba esta hardcodeada";
                void* bufferEncontrado=hardcodeo;

                t_paquete* paqueteDevolverAIO=crear_paquete(IO_MEM_STDOUT_WRITE);
                agregar_a_paquete(paqueteDevolverAIO,bufferEncontrado,tamanio);
                enviar_paquete(paqueteDevolverAIO,socketCliente);
                free(paqueteDevolverAIO->buffer);
                free(paqueteDevolverAIO);
                break;
                
            }

            default:
            {   
                //log_error(loggerMemoria, "Error");
                break;
            }

        }

        eliminar_paquete(paquete);
    }

}