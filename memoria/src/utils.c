#include <utils.h>

char* leer_string(char *buffer, int *desplazamiento) {
    uint32_t tamanio = leer_entero(buffer, &desplazamiento);

    char *valor = malloc(tamanio + 1); // Allocate memory for the string plus a null terminator

    memcpy(valor, buffer + (*desplazamiento), tamanio);
    valor[tamanio] = '\0'; // Null-terminate the string
    (*desplazamiento) += tamanio;

    return valor;
}



