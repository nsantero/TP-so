#include <instrucciones.h>

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
    uint32_t *reg_destino = NULL;
    uint32_t *reg_origen = NULL;

    // Asignar punteros a los registros correspondientes
    if (strcmp(destino, "AX") == 0) reg_destino = (uint32_t*)&cpu->AX;
    else if (strcmp(destino, "BX") == 0) reg_destino = (uint32_t*)&cpu->BX;
    else if (strcmp(destino, "CX") == 0) reg_destino = (uint32_t*)&cpu->CX;
    else if (strcmp(destino, "DX") == 0) reg_destino = (uint32_t*)&cpu->DX;
    else if (strcmp(destino, "EAX") == 0) reg_destino = &cpu->EAX;
    else if (strcmp(destino, "EBX") == 0) reg_destino = &cpu->EBX;
    else if (strcmp(destino, "ECX") == 0) reg_destino = &cpu->ECX;
    else if (strcmp(destino, "EDX") == 0) reg_destino = &cpu->EDX;

    if (strcmp(origen, "AX") == 0) reg_origen = (uint32_t*)&cpu->AX;
    else if (strcmp(origen, "BX") == 0) reg_origen = (uint32_t*)&cpu->BX;
    else if (strcmp(origen, "CX") == 0) reg_origen = (uint32_t*)&cpu->CX;
    else if (strcmp(origen, "DX") == 0) reg_origen = (uint32_t*)&cpu->DX;
    else if (strcmp(origen, "EAX") == 0) reg_origen = &cpu->EAX;
    else if (strcmp(origen, "EBX") == 0) reg_origen = &cpu->EBX;
    else if (strcmp(origen, "ECX") == 0) reg_origen = &cpu->ECX;
    else if (strcmp(origen, "EDX") == 0) reg_origen = &cpu->EDX;

    // Realizar la resta si ambos registros son válidos
    if (reg_destino && reg_origen) {
        *reg_destino -= *reg_origen;
        printf("SUB - %s = %d\n", destino, *reg_destino);
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

void ejecutar_wait(CPU_Registers *cpu, const char* recurso) {
    // Enviar la solicitud de WAIT al Kernel
    //enviar_solicitud_wait(recurso);
    printf("WAIT - Solicitud enviada para el recurso: %s\n", recurso);
}

void paquete_solicitud_wait(const char* recurso) {
    t_paquete *paquete_CPU_solicitud_wait = crear_paquete(PROCESO_WAIT);
    //agregar_entero_a_paquete32(paquete_CPU_solicitud_wait, proceso->PID);
    
    //enviar_paquete(paquete_CPU_solicitud_wait, cpu_dispatch_fd);
    eliminar_paquete(paquete_CPU_solicitud_wait);
    printf("Enviando solicitud WAIT al Kernel para el recurso: %s\n", recurso);
}

// Falta COPY_STRING, RESIZE, MOV_OUT, MOV_IN