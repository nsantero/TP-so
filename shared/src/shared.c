#include <shared.h>

void decir_hola(char* quien) {
   printf("Hola desde %s!!\n", quien);
}

int iniciar_servidor(t_log *logger,char* server_name, char* puerto)
{

    int socketServidor;

	struct addrinfo hints, *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, puerto, &hints, &serverInfo);
	if(status){
		fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(status)); // cargar al logger
        exit(EXIT_FAILURE);
	}

	// Creamos el socket de escucha del servidor
	socketServidor = socket(serverInfo->ai_family,
	 						serverInfo->ai_socktype,
	  						serverInfo->ai_protocol);

	if(socketServidor  == -1){
		close(socketServidor);
		exit(EXIT_FAILURE);
	}
						
	// Asociamos el socket a un puerto

	if(bind(socketServidor, serverInfo->ai_addr,serverInfo->ai_addrlen) == -1){
		close(socketServidor);
		exit(EXIT_FAILURE);
	}

	// Escuchamos las conexiones entrantes

	if(listen(socketServidor,SOMAXCONN) == -1){
		close(socketServidor);
		exit(EXIT_FAILURE);
	}
	
	freeaddrinfo(serverInfo);

	log_info(logger, "Servidor iniciado");
	printf("Servidor iniciado %s\n", server_name );

	return socketServidor;

}
int iniciarServidorV2(t_log *logger, char* puerto)
{

    int socketServidor;

	struct addrinfo hints, *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(NULL, puerto, &hints, &serverInfo);
	if(status){
		fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(status)); // cargar al logger
        exit(EXIT_FAILURE);
	}

	// Creamos el socket de escucha del servidor
	socketServidor = socket(serverInfo->ai_family,
	 						serverInfo->ai_socktype,
	  						serverInfo->ai_protocol);

	if(socketServidor  == -1){
		close(socketServidor);
		exit(EXIT_FAILURE);
	}
						
	// Asociamos el socket a un puerto

	if(bind(socketServidor, serverInfo->ai_addr,serverInfo->ai_addrlen) == -1){
		close(socketServidor);
		exit(EXIT_FAILURE);
	}

	// Escuchamos las conexiones entrantes

	if(listen(socketServidor,SOMAXCONN) == -1){
		close(socketServidor);
		exit(EXIT_FAILURE);
	}
	
	freeaddrinfo(serverInfo);

	log_info(logger, "Servidor iniciado");
	printf("Servidor iniciado\n" );

	return socketServidor;

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

/*char* recibir_mensaje(int socket_cliente) {
    int size;
    char* buffer = (char*)recibir_buffer(&size, socket_cliente);
    if (buffer != NULL) {
        return buffer;
    }
}*/

int esperarClienteV2(t_log* logger, int socketServidor){
	int socketCliente = 0;

	socketCliente = accept(socketServidor, NULL, NULL);

	log_info(logger, "Se conecto un cliente");

	return socketCliente;
}

int esperar_cliente(t_log* logger, const char* name, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

//    log_info(logger,ANSI_COLOR_GREEN "Cliente conectado a %s", name);

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
		printf("Conexion incorrecta con: %s\n", server_name);
        freeaddrinfo(server_info);
        return 0;
    }
    else
        log_info(logger,ANSI_COLOR_GREEN "Me conecte en %s:%s (a %s)\n", ip, puerto, server_name);
		printf("Conexion correcta con: %s\n", server_name);

    freeaddrinfo(server_info);

    return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
    close(socket_cliente);
    socket_cliente = -1;
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