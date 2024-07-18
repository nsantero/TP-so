#include <paquetes.h>

// CREACION DE PAQUETES //

t_paquete* crear_paquete(op_code codigo_op){
    t_paquete* paquete = malloc(sizeof(op_code)+sizeof(t_buffer));
    paquete->codigo_operacion = codigo_op;
    crear_buffer(paquete);
    return paquete;
}

void crear_buffer(t_paquete* paquete){
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;
}

t_buffer* crear_buffer_aislado(void* data, int data_size){ //
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->stream = malloc(data_size);
    memcpy(buffer->stream, data, data_size);
    buffer->size = data_size;
	return buffer;
}

// AGREGAR DATOS A PAQUETES //

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio){
    void* newStream = NULL;
    newStream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));
    if(newStream == NULL){
        free(paquete->buffer->stream);
        exit(EXIT_FAILURE);
    }
    paquete->buffer->stream = newStream;

    memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
    memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

    paquete->buffer->size += tamanio + sizeof(int);
}

void agregar_entero_a_paquete32(t_paquete *paquete, uint32_t x)
{
    void* newStream = NULL;
    newStream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint32_t));
    if(newStream == NULL){
        free(paquete->buffer->stream);
        exit(EXIT_FAILURE);
    }
    paquete->buffer->stream = newStream;
    memcpy(paquete->buffer->stream + paquete->buffer->size, &x, sizeof(uint32_t));
    paquete->buffer->size += sizeof(uint32_t);
}
void agregar_entero_a_paquete8(t_paquete *paquete, uint8_t x)
{
    void* newStream = NULL;
    newStream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint8_t));
    if(newStream == NULL){
        free(paquete->buffer->stream);
        exit(EXIT_FAILURE);
    }
    paquete->buffer->stream = newStream;
    memcpy(paquete->buffer->stream + paquete->buffer->size, &x, sizeof(uint8_t));
    paquete->buffer->size += sizeof(uint8_t);
}

void agregar_string_a_paquete(t_paquete *paquete, char* palabra)
{    void* newStream = NULL;
    size_t palabra_lenght = strlen(palabra) + 1;
    newStream = realloc(paquete->buffer->stream, paquete->buffer->size + palabra_lenght);
    if(newStream == NULL){
        free(paquete->buffer->stream);
        exit(EXIT_FAILURE);
    }
    paquete->buffer->stream = newStream;
    memcpy(paquete->buffer->stream + paquete->buffer->size, palabra, palabra_lenght);
    paquete->buffer->size += (palabra_lenght);
}

// ELIMINAR PAQUETES //

void eliminar_paquete(t_paquete* paquete) {
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

// LEER PAQUETES //

uint32_t leer_entero(char *buffer, int *desplazamiento) {
    //uint32_t* ret= malloc(sizeof(uint32_t));
    uint32_t ret;
    memcpy(&ret, buffer + (*desplazamiento), sizeof(uint32_t));
    (*desplazamiento) += sizeof(uint32_t);
    return ret;
}

// SERIALIZACION DE PAQUETES //

void* serializar_paquete(t_paquete* paquete, int bytes) {
    void * magic = NULL;
    magic = malloc(bytes);
    int desplazamiento = 0;

    memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(op_code));
    desplazamiento+= sizeof(op_code);
    memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
    desplazamiento+= sizeof(int);
    memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    desplazamiento+= paquete->buffer->size;

    return magic; 
}
