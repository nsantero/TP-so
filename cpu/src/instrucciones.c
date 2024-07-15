#include "../../kernel/src/kernel.h"

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

    // Realizar la suma si ambos registros son válidos
    if (reg_destino && reg_origen) {
        *reg_destino += *reg_origen;
        printf("SUM - %s = %d\n", destino, *reg_destino);
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