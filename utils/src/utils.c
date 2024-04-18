#include "../include/utils.h"


void decir_hola(char* quien) {
   printf("Hola desde %s!!\n", quien);
}


int iniciar_servidor(t_log* logger,  char* ip, char* puerto) {
   int socket_servidor;
   struct addrinfo hints, *servinfo;


   // Inicializando hints
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;


   // Recibe los addrinfo
   getaddrinfo(ip, puerto, &hints, &servinfo);


   bool conecto = false;


   // Itera por cada addrinfo devuelto
   for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
       socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
       if (socket_servidor == -1) // fallo de crear socket
           continue;


       if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
           // Si entra aca fallo el bind
           close(socket_servidor);
           continue;
       }
       // Ni bien conecta uno nos vamos del for
       conecto = true;
       break;
   }


   if(!conecto) {
       free(servinfo);
       return 0;
   }


   listen(socket_servidor, SOMAXCONN); // Escuchando (hasta SOMAXCONN conexiones simultaneas)


   // Aviso al logger
   log_trace(logger, "Escuchando en %s:%s \n", ip, puerto);


   freeaddrinfo(servinfo); //free


   return socket_servidor;
}


int crear_conexion(t_log* logger, char* ip, char* puerto) {
   struct addrinfo hints, *servinfo;


   // Init de hints
   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;


   // Recibe addrinfo
   getaddrinfo(ip, puerto, &hints, &servinfo);


   // Crea un socket con la informacion recibida (del primero, suficiente)
   int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);


   // Fallo en crear el socket
   if(socket_cliente == -1) {
       log_error(logger, "Error creando el socket para %s:%s", ip, puerto);
       return 0;
   }


   // Error conectando
   if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
       log_error(logger, "Error al conectarse");
       freeaddrinfo(servinfo);
       return 0;
   } else
       log_info(logger, "Cliente conectado en %s:%s \n", ip, puerto);


   freeaddrinfo(servinfo);


   return socket_cliente;
}




/*int server_escuchar(t_log* logger, char* server_name, int server_socket) {
   int cliente_socket = esperar_cliente(logger, server_name, server_socket);


   if (cliente_socket != -1) {
       pthread_t hilo;
       t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
       args->log = logger;
       args->fd = cliente_socket;
       args->server_name = server_name;
       pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
       pthread_detach(hilo);
       return 1;
   }
   return 0;
}*/
