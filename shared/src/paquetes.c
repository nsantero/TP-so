#include <paquetes.h>

// CREACION DE PAQUETES //

t_paquete* crear_paquete(op_code codigo_op){
	t_paquete* paquete = malloc(sizeof(t_paquete));
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
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void agregar_entero_a_paquete32(t_paquete *paquete, uint32_t x)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint32_t));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &x, sizeof(uint32_t));
	paquete->buffer->size += sizeof(uint32_t);
}
void agregar_entero_a_paquete8(t_paquete *paquete, uint8_t x)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint8_t));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &x, sizeof(uint8_t));
	paquete->buffer->size += sizeof(uint8_t);
}

void agregar_string_a_paquete(t_paquete *paquete, char* palabra)
{	
	size_t palabra_lenght = strlen(palabra) + 1;
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + palabra_lenght);
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
	int size_paquete = sizeof(int) + sizeof(int) + paquete->buffer->size;
    void* paquete_serializado = NULL;

	paquete_serializado = malloc(size_paquete); // TODO: need free (3)
	int offset = 0;
	memcpy(paquete_serializado + offset, &(paquete->codigo_operacion), sizeof(int));

	offset += sizeof(int);
	memcpy(paquete_serializado + offset, &(paquete->buffer->size), sizeof(int));

	offset += sizeof(int);
	memcpy(paquete_serializado + offset, paquete->buffer->stream, paquete->buffer->size);

	return paquete_serializado; 
}
