#include <instrucciones.h>
#include <math.h>

void ejecutar_set(CPU_Registers *registros,  char* registro, uint32_t valor) {
    if (strcmp(registro, "AX") == 0) {
        registros->AX = valor;
    } else if (strcmp(registro, "BX") == 0) {
        registros->BX = valor;
    } else if (strcmp(registro, "CX") == 0) {
        registros->CX = valor;
    } else if (strcmp(registro, "DX") == 0) {
        registros->DX = valor;
    } else if (strcmp(registro, "EAX") == 0) {
        registros->EAX = valor;
    } else if (strcmp(registro, "EBX") == 0) {
        registros->EBX = valor;
    } else if (strcmp(registro, "ECX") == 0) {
        registros->ECX = valor;
    } else if (strcmp(registro, "EDX") == 0) {
        registros->EDX = valor;
    } else if (strcmp(registro, "PC") == 0) {
        registros->PC = valor;
    }else if (strcmp(registro, "SI") == 0) {
        registros->SI = valor;
    }else if (strcmp(registro, "DI") == 0) {
        registros->DI = valor;
    } else {
        printf("Registro no reconocido: %s\n", registro);
    }
}

void ejecutar_sum(CPU_Registers *cpu,  char* destino,  char* origen) {
    uint32_t *reg_destino_32 = NULL;
    uint8_t *reg_destino_8 = NULL;
    uint32_t *reg_origen_32 = NULL;
    uint8_t *reg_origen_8 = NULL;

    // Asignar punteros a los registros correspondientes
    if (strcmp(destino, "AX") == 0) reg_destino_8 = &cpu->AX;
    else if (strcmp(destino, "BX") == 0) reg_destino_8 = &cpu->BX;
    else if (strcmp(destino, "CX") == 0) reg_destino_8 = &cpu->CX;
    else if (strcmp(destino, "DX") == 0) reg_destino_8 = &cpu->DX;
    else if (strcmp(destino, "EAX") == 0) reg_destino_32 = &cpu->EAX;
    else if (strcmp(destino, "EBX") == 0) reg_destino_32 = &cpu->EBX;
    else if (strcmp(destino, "ECX") == 0) reg_destino_32 = &cpu->ECX;
    else if (strcmp(destino, "EDX") == 0) reg_destino_32 = &cpu->EDX;

    if (strcmp(origen, "AX") == 0) reg_origen_8 = &cpu->AX;
    else if (strcmp(origen, "BX") == 0) reg_origen_8 = &cpu->BX;
    else if (strcmp(origen, "CX") == 0) reg_origen_8 = &cpu->CX;
    else if (strcmp(origen, "DX") == 0) reg_origen_8 = &cpu->DX;
    else if (strcmp(origen, "EAX") == 0) reg_origen_32 = &cpu->EAX;
    else if (strcmp(origen, "EBX") == 0) reg_origen_32 = &cpu->EBX;
    else if (strcmp(origen, "ECX") == 0) reg_origen_32 = &cpu->ECX;
    else if (strcmp(origen, "EDX") == 0) reg_origen_32 = &cpu->EDX;

    // Realizar la suma si ambos registros son válidos
    if (reg_destino_8 && reg_origen_8) {
        *reg_destino_8 += *reg_origen_8;
        printf("SUM - %s = %d\n", destino, *reg_destino_8);
    } else if (reg_destino_32 && reg_origen_32) {
        *reg_destino_32 += *reg_origen_32;
        printf("SUM - %s = %d\n", destino, *reg_destino_32);
    } else {
        printf("Registro no reconocido: %s o %s\n", destino, origen);
    }
}

void ejecutar_sub(CPU_Registers *cpu,  char* destino,  char* origen) {
    uint32_t *reg_destino_32 = NULL;
    uint8_t *reg_destino_8 = NULL;
    uint32_t *reg_origen_32 = NULL;
    uint8_t *reg_origen_8 = NULL;

    // Asignar punteros a los registros correspondientes
    if (strcmp(destino, "AX") == 0) reg_destino_8 = &cpu->AX;
    else if (strcmp(destino, "BX") == 0) reg_destino_8 = &cpu->BX;
    else if (strcmp(destino, "CX") == 0) reg_destino_8 = &cpu->CX;
    else if (strcmp(destino, "DX") == 0) reg_destino_8 = &cpu->DX;
    else if (strcmp(destino, "EAX") == 0) reg_destino_32 = &cpu->EAX;
    else if (strcmp(destino, "EBX") == 0) reg_destino_32 = &cpu->EBX;
    else if (strcmp(destino, "ECX") == 0) reg_destino_32 = &cpu->ECX;
    else if (strcmp(destino, "EDX") == 0) reg_destino_32 = &cpu->EDX;

    if (strcmp(origen, "AX") == 0) reg_origen_8 = &cpu->AX;
    else if (strcmp(origen, "BX") == 0) reg_origen_8 = &cpu->BX;
    else if (strcmp(origen, "CX") == 0) reg_origen_8 = &cpu->CX;
    else if (strcmp(origen, "DX") == 0) reg_origen_8 = &cpu->DX;
    else if (strcmp(origen, "EAX") == 0) reg_origen_32 = &cpu->EAX;
    else if (strcmp(origen, "EBX") == 0) reg_origen_32 = &cpu->EBX;
    else if (strcmp(origen, "ECX") == 0) reg_origen_32 = &cpu->ECX;
    else if (strcmp(origen, "EDX") == 0) reg_origen_32 = &cpu->EDX;

    // Realizar la resta si ambos registros son válidos
    if (reg_destino_8 && reg_origen_8) {
        *reg_destino_8 -= *reg_origen_8;
        printf("SUB - %s = %d\n", destino, *reg_destino_8);
    } else if (reg_destino_32 && reg_origen_32) {
        *reg_destino_32 -= *reg_origen_32;
        printf("SUB - %s = %d\n", destino, *reg_destino_32);
    } else {
        printf("Registro no reconocido: %s o %s\n", destino, origen);
    }
}

void ejecutar_jnz(CPU_Registers *cpu, char* registro, uint32_t nueva_instruccion) {
    uint32_t valor_registro = 0;

    // Asignar valor del registro correspondiente
    if (strcmp(registro, "AX") == 0) valor_registro = cpu->AX;
    else if (strcmp(registro, "BX") == 0) valor_registro = cpu->BX;
    else if (strcmp(registro, "CX") == 0) valor_registro = cpu->CX;
    else if (strcmp(registro, "DX") == 0) valor_registro = cpu->DX;
    else if (strcmp(registro, "EAX") == 0) valor_registro = cpu->EAX;
    else if (strcmp(registro, "EBX") == 0) valor_registro = cpu->EBX;
    else if (strcmp(registro, "ECX") == 0) valor_registro = cpu->ECX;
    else if (strcmp(registro, "EDX") == 0) valor_registro = cpu->EDX;
    else {
        printf("Registro no reconocido: %s\n", registro);
        return;
    }

    // Si el valor del registro es distinto de cero, actualizar el PC
    if (valor_registro != 0) {
        cpu->PC = nueva_instruccion;
        printf("JNZ - Registro %s distinto de cero, saltando a instrucción %d\n", registro, nueva_instruccion);
    } else {
        printf("JNZ - Registro %s es cero, no se realiza salto\n", registro);
    }
}



void paquete_kernel_envio_recurso_wait(char* recurso){

    t_paquete *paquete_kernel_recurso = crear_paquete(PROCESO_WAIT);
    
    agregar_entero_a_paquete32(paquete_kernel_recurso, (strlen(recurso)+1));
    agregar_string_a_paquete(paquete_kernel_recurso, recurso);
    
    enviar_paquete(paquete_kernel_recurso, socketCliente);
    eliminar_paquete(paquete_kernel_recurso);

}

void paquete_kernel_envio_recurso_signal(char* recurso){

    t_paquete *paquete_kernel_recurso = crear_paquete(PROCESO_SIGNAL);
    
    agregar_entero_a_paquete32(paquete_kernel_recurso, (strlen(recurso)+1));
    agregar_string_a_paquete(paquete_kernel_recurso, recurso);
    
    enviar_paquete(paquete_kernel_recurso, socketCliente);
    eliminar_paquete(paquete_kernel_recurso);

}

int ejecutar_wait(Proceso *procesoActual, char* recurso) {
    
    int bloqueado = 0;
    paquete_kernel_envio_recurso_wait(recurso);
    int respuesta = recibir_resultado_recursos();
    if (respuesta == 0) {
        bloqueado = 1;
        mandarPaqueteaKernel(WAIT_BLOCK);
    }
    return bloqueado;
}


int ejecutar_signal(Proceso *procesoActual, char* recurso) {
    int bloqueado = 0;
    paquete_kernel_envio_recurso_signal(recurso);
    int respuesta = recibir_resultado_recursos(WAIT_BLOCK);
    if (respuesta == 0) {
        bloqueado = 1;
    }
    return bloqueado;
}

int recibir_resultado_recursos(){

    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    recv(socketCliente, &(paquete->codigo_operacion), sizeof(op_code), 0);
    recv(socketCliente, &(paquete->buffer->size), sizeof(int), 0);
    
    int respuesta;

    switch(paquete->codigo_operacion){
        case WAIT_SUCCESS:
        {   
            respuesta = 1;
            printf("Se pudo hacer el wait \n");
            break;
        }
        case WAIT_BLOCK:
        {
            respuesta = 0;
            printf("No se puede hacer el wait, devuelvo el proceso \n");
            break;
        }
        case SIGNAL_SUCCESS:
            respuesta = 1;
            printf("Se pudo hacer el signal \n");
            break;
        default:
        {   
            log_error(loggerCpu, "Error el codigo no es correcto");
            break;
        }
      
    }       
    return respuesta;
}
void ejecutarCopyString(Proceso *procesoEjecutando, int size_dato){
    

    int pid=procesoEjecutando->PID;
    uint32_t dirLogOriginal=leerValorDelRegistro("SI",procesoEjecutando->cpuRegisters);
    uint32_t dirLogDestino=leerValorDelRegistro("DI",procesoEjecutando->cpuRegisters);
    direccion_fisica *direccion_fisica;          
    void* bufferStrcpy=NULL;
    bufferStrcpy=malloc(size_dato);
    int cantidadDePaginas = 0;    
    uint32_t bytesDisponiblesEnPag;
    int nro_pagina =0;
    int tam =0;
    void* datos_leidos=NULL;

    //-----------------------RECIBE EL STRING A COPIAR-----------------------------------//
    direccion_fisica = traduccion_mmu(dirLogOriginal,pid);
    cantidadDePaginas = calculo_cantiad_paginas(dirLogOriginal,pid,direccion_fisica->desplazamiento,size_dato);
    bytesDisponiblesEnPag = tam_pagina-direccion_fisica->desplazamiento;

    if(size_dato<=bytesDisponiblesEnPag){
        tam=size_dato;
    }else{
        tam=bytesDisponiblesEnPag;
    }
    datos_leidos = enviar_paquete_mov_in_memoria(direccion_fisica->PID,direccion_fisica->numero_frame,direccion_fisica->desplazamiento,tam);
    //datos_leidos = recibir_confirmacion_memoria_mov_in();
    memcpy(bufferStrcpy, datos_leidos,tam);
    free(datos_leidos);
    direccion_fisica->desplazamiento = 0;
    for(int i=1; i<cantidadDePaginas;i++){

        nro_pagina = floor(dirLogOriginal / tam_pagina)+i; 
        direccion_fisica->numero_frame = buscar_frame(nro_pagina,pid);
        
        if (i<(cantidadDePaginas-1)){

            datos_leidos = enviar_paquete_mov_in_memoria(direccion_fisica->PID,direccion_fisica->numero_frame,direccion_fisica->desplazamiento,tam_pagina);
            //datos_leidos = recibir_confirmacion_memoria_mov_in();
            memcpy(bufferStrcpy +tam , datos_leidos, tam_pagina);
            tam+=tam_pagina;
            
            free(datos_leidos);

        }else{
            
            datos_leidos= enviar_paquete_mov_in_memoria(direccion_fisica->PID,direccion_fisica->numero_frame,direccion_fisica->desplazamiento,size_dato-tam);
            //datos_leidos = recibir_confirmacion_memoria_mov_in();
            memcpy(bufferStrcpy+tam, datos_leidos, size_dato-tam);
            free(datos_leidos);

        }
        
    }


    //----------------------MANDA EL STRING A MEMORIA--------------------------------//

    
    void* buffer=NULL;
    int operacion;

    
    direccion_fisica = traduccion_mmu(dirLogDestino,pid);
    cantidadDePaginas = calculo_cantiad_paginas(dirLogDestino,pid,direccion_fisica->desplazamiento,size_dato);

    for(int i=0; i<cantidadDePaginas;i++){
        nro_pagina = floor(dirLogDestino / tam_pagina)+i;                 
        direccion_fisica->desplazamiento = 0;
        direccion_fisica->PID = pid;
        direccion_fisica->numero_frame = buscar_frame(nro_pagina,pid);
        
        if (i==0) {
            direccion_fisica->desplazamiento = dirLogDestino - (nro_pagina * tam_pagina);
            bytesDisponiblesEnPag = tam_pagina-direccion_fisica->desplazamiento;
            if(size_dato<=bytesDisponiblesEnPag){
                tam=size_dato;
            }else{
                tam=bytesDisponiblesEnPag;
            }

            buffer=malloc(tam);
            memcpy(buffer,bufferStrcpy,tam);
            operacion = enviar_paquete_mov_out_memoria(direccion_fisica->PID,direccion_fisica->numero_frame,direccion_fisica->desplazamiento,tam,buffer);

            if (operacion == -1){}
            //memcpy(loQueDevuelve, buffer,tam);
            free(buffer);

        }else if (i<(cantidadDePaginas-1)){

            buffer=malloc(tam_pagina);
            memcpy(buffer,bufferStrcpy+tam,tam_pagina);
            operacion = enviar_paquete_mov_out_memoria(direccion_fisica->PID,direccion_fisica->numero_frame,direccion_fisica->desplazamiento,tam_pagina,buffer);
            tam+=tam_pagina;                   
            if (operacion == -1){}
            //memcpy(loQueDevuelve +tam+(tam_pagina*(i-1)) , buffer, tam_pagina);
            free(buffer);

        }else{
            buffer=malloc(size_dato-tam);
            memcpy(buffer,bufferStrcpy+tam,size_dato-tam);
            operacion = enviar_paquete_mov_out_memoria(direccion_fisica->PID,direccion_fisica->numero_frame,direccion_fisica->desplazamiento,size_dato-tam,buffer);
            //datos_leidos = recibir_confirmacion_memoria_mov_out();

            if (operacion == -1){}
            //memcpy(loQueDevuelve+tam+(tam_pagina*(i-1)), buffer, size_dato-tam);
            free(buffer);
        }
        
    }


   
 
}


