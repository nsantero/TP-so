#include <instrucciones.h>
#include <math.h>

void ejecutar_set(CPU_Registers *registros, const char* registro, uint8_t valor) {
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
    } else {
        printf("Registro no reconocido: %s\n", registro);
    }
}

void ejecutar_sum(CPU_Registers *cpu, const char* destino, const char* origen) {
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

void ejecutar_sub(CPU_Registers *cpu, const char* destino, const char* origen) {
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

void ejecutar_jnz(CPU_Registers *cpu, const char* registro, uint32_t nueva_instruccion) {
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



void paquete_kernel_envio_recurso_wait(const char* recurso){

    t_paquete *paquete_kernel_recurso = crear_paquete(PROCESO_WAIT);
    
    agregar_entero_a_paquete32(paquete_kernel_recurso, (strlen(recurso)+1));
    agregar_string_a_paquete(paquete_kernel_recurso, recurso);
    
    enviar_paquete(paquete_kernel_recurso, socketCliente);
    eliminar_paquete(paquete_kernel_recurso);

}

void paquete_kernel_envio_recurso_signal(const char* recurso){

    t_paquete *paquete_kernel_recurso = crear_paquete(PROCESO_SIGNAL);
    
    agregar_entero_a_paquete32(paquete_kernel_recurso, (strlen(recurso)+1));
    agregar_string_a_paquete(paquete_kernel_recurso, recurso);
    
    enviar_paquete(paquete_kernel_recurso, socketCliente);
    eliminar_paquete(paquete_kernel_recurso);

}

int ejecutar_wait(Proceso *procesoActual, const char* recurso) {
    
    int bloqueado = 0;
    paquete_kernel_envio_recurso_wait(recurso);
    int respuesta = recibir_resultado_recursos();
    if (respuesta == 0) {
        bloqueado = 1;
        mandarPaqueteaKernel(WAIT_BLOCK);
    }
    return bloqueado;
}


int ejecutar_signal(Proceso *procesoActual, const char* recurso) {
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
void ejecutarCopyString(Proceso *procesoEjecutando, int sizeDato){
    int nro_pagina =0;
    int tam =0;
    void* buffer;
    void* datos_leidos=NULL;

    void* datos_a_escribir = &procesoEjecutando->cpuRegisters.SI;

    direccion_fisica* dirFis = traduccion_mmu(procesoEjecutando->cpuRegisters.DI, procesoEjecutando->PID);

    int cantidadDePaginas = calculo_cantiad_paginas(procesoEjecutando->cpuRegisters.DI,procesoEjecutando->PID,dirFis->desplazamiento,sizeDato);

    uint32_t bytesDisponiblesEnPag;
            
    for(int i=0; i<cantidadDePaginas;i++){

        nro_pagina = floor(procesoEjecutando->cpuRegisters.DI / tam_pagina)+i; 
        
        dirFis->desplazamiento = 0;
        dirFis->PID = procesoEjecutando;

        dirFis->numero_frame = buscar_frame(nro_pagina,procesoEjecutando->PID);
        

        if (i==0) {
            dirFis->desplazamiento = procesoEjecutando->cpuRegisters.DI - (nro_pagina * tam_pagina);
            bytesDisponiblesEnPag = tam_pagina-dirFis->desplazamiento;
            if(sizeDato<=bytesDisponiblesEnPag){
                tam=sizeDato;
            }else{
                tam=bytesDisponiblesEnPag;
            }
            buffer=malloc(tam);
            memcpy(buffer,datos_a_escribir,tam);
            enviar_paquete_mov_out_memoria(dirFis->PID,dirFis->numero_frame,dirFis->desplazamiento,tam,buffer);
            datos_leidos = recibir_confirmacion_memoria_mov_out();
            free(buffer);
        }else if (i<(cantidadDePaginas-1)){
            buffer=malloc(tam_pagina);
            memcpy(buffer,datos_a_escribir+tam,tam_pagina);
            enviar_paquete_mov_out_memoria(dirFis->PID,dirFis->numero_frame,dirFis->desplazamiento,tam_pagina,buffer);
            tam+=tam_pagina;
            datos_leidos = recibir_confirmacion_memoria_mov_out();
            free(buffer);
        }else{
            buffer=malloc(sizeDato-tam);
            memcpy(buffer,datos_a_escribir+tam,sizeDato-tam);
            enviar_paquete_mov_out_memoria(dirFis->PID,dirFis->numero_frame,dirFis->desplazamiento,sizeDato-tam,buffer);
            datos_leidos = recibir_confirmacion_memoria_mov_out();
            free(buffer);
        }
    }
}


