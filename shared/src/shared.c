#include "../include/shared.h"




void decir_hola(char* quien) {
   printf("Hola desde %s!!\n", quien);
}

int iniciar_servidor(t_log *logger,char* nombre, char *ip, char* puerto)
{
	struct addrinfo hints, *server_info;
	int socket_servidor, s;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	s = getaddrinfo(ip, puerto, &hints, &server_info);
	if(s!=0){
		exit(EXIT_FAILURE);
	}

	socket_servidor = socket(server_info->ai_family,
	                    server_info->ai_socktype,
	                    server_info->ai_protocol);

	if(socket_servidor == -1){
		close(socket_servidor);
		exit(EXIT_FAILURE);
	}

	if(bind(socket_servidor, server_info->ai_addr, server_info->ai_addrlen) == -1){
		close(socket_servidor);
		exit(EXIT_FAILURE);
	}

	if(listen(socket_servidor, SOMAXCONN) == -1){
		close(socket_servidor);
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(server_info);
    log_info(logger, ANSI_COLOR_GREEN "Escuchando en %s", nombre);
	log_info(logger, "Servidor iniciado");

	return socket_servidor;
} 

void* recibir_buffer(int* size, int socket_cliente){
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void enviar_mensaje(char* mensaje, int socket_cliente){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

char* recibir_mensaje(int socket_cliente) {
    int size;
    char* buffer = (char*)recibir_buffer(&size, socket_cliente);
    if (buffer != NULL) {
        return buffer;
    }
}

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


void eliminar_paquete(t_paquete* paquete) {
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

int esperar_cliente(t_log* logger, const char* name, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

    log_info(logger,ANSI_COLOR_GREEN "Cliente conectado a %s", name);

    return socket_cliente;
}

int crear_conexion(t_log *logger, const char *server_name, char *ip, char *puerto)
{
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &server_info);

    // Ahora vamos a crear el socket.
    int socket_cliente = socket(server_info->ai_family,
                                server_info->ai_socktype,
                                server_info->ai_protocol);

    // Ahora que tenemos el socket, vamos a conectarlo
    // Fallo en crear el socket
    if (socket_cliente == -1)
    {
        log_error(logger, "Error creando el socket para %s:%s", ip, puerto);
        return 0;
    }

    // Error conectando
    if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        perror("Error al conectarse");
        log_error(logger, "Error al conectarme (a %s)\n", server_name);
        freeaddrinfo(server_info);
        return 0;
    }
    else
        log_info(logger,ANSI_COLOR_GREEN "Me conecte en %s:%s (a %s)\n", ip, puerto, server_name);

    freeaddrinfo(server_info);

    return socket_cliente;
}
void liberar_conexion(int socket_cliente)
{
    close(socket_cliente);
    socket_cliente = -1;
}

void agregar_entero_a_paquete(t_paquete *paquete, uint32_t x)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(uint32_t));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &x, sizeof(uint32_t));
	paquete->buffer->size += sizeof(uint32_t);
}

void crear_buffer(t_paquete* paquete){
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(op_code codigo_op){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_op;
	crear_buffer(paquete);
	return paquete;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio){
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}


t_list* recibir_paquete(int socket_cliente){
	int size;
	int desplazamiento = 0;
	void* buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	//memory leaks
	if (buffer == NULL) {
        // Manejar el error de recepción del paquete
        list_destroy(valores);
        return NULL;
    }

	while(desplazamiento < size){
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente){
	int bytes = paquete->buffer->size + sizeof(int) + sizeof(op_code);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void agregar_string_a_paquete(t_paquete *paquete, char* palabra)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + sizeof(char*));
	memcpy(paquete->buffer->stream + paquete->buffer->size, &palabra, sizeof(char*));
	paquete->buffer->size += (sizeof(char*));
}




uint32_t leer_entero(char *buffer, int *desplazamiento) {
    //uint32_t* ret= malloc(sizeof(uint32_t));
    uint32_t ret;
    memcpy(&ret, buffer + (*desplazamiento), sizeof(uint32_t));
    (*desplazamiento) += sizeof(uint32_t);
    return ret;
}


