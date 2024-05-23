#include "shared.h"

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

t_buffer* crear_buffer(void) {
    t_buffer* nuevoBuffer = malloc(sizeof(*nuevoBuffer));
    nuevoBuffer->size = 0;
    nuevoBuffer->stream = NULL;
    return nuevoBuffer;
}

void* recibir_buffer(int* size, int socket_cliente){
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

uint8_t stream_recv_de_header(int fromSocket) {
    uint8_t header;
    ssize_t msgBytes = recv(fromSocket, &header, sizeof(header), 0);
    if (msgBytes == -1) {
        printf("\e[0;31mstream_recv_buffer: Error en la recepción del header [%s]\e[0m\n", strerror(errno));
    }
    return header;
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




