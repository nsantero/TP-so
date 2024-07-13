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
    } else {
        printf("Registro no reconocido: %s\n", registro);
    }
}