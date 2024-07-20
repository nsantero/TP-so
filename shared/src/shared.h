#ifndef SHARED_H
#define SHARED_H

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <assert.h>
#include <pthread.h>
#include <netdb.h>
#include <paquetes.h>
# define IP	"127.0.0.1"

#define ANSI_COLOR_GREEN   "\x1b[32m" // Código de escape para color verde
#define ANSI_COLOR_BLUE    "\x1b[34m" // Código de escape para color azul
#define ANSI_COLOR_YELLOW  "\x1b[33m" // Código de escape para color amarillo
#define ANSI_COLOR_RESET   "\x1b[0m"  // Código de escape para resetear el color
#define ANSI_COLOR_GRAY    "\x1b[90m"  // Código de escape para color gris
#define ANSI_COLOR_CYAN    "\x1b[36m"  // Código de escape para color celeste
#define ANSI_COLOR_PINK    "\x1b[95m"  // Código de escape para color rosa pastel
#define ANSI_COLOR_LIME    "\x1b[92m"






/**
* @fn    decir_hola
* @brief Imprime un saludo al nombre que se pase por parámetro por consola.
*/


void decir_hola(char* quien);
int iniciar_servidor(t_log *logger,char* nombre, char* puerto);
int iniciarServidorV2(t_log *logger, char* puerto);

int crear_conexion(t_log *logger, const char *server_name, char *ip, char *puerto);
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);
int esperarClienteV2(t_log* logger, int socketServidor);

void liberar_conexion(int socket_cliente);

void enviar_paquete(t_paquete* paquete, int socket_cliente);
int enviar_paquete_interfaces(t_paquete* paquete, int socket_cliente);
void enviar_mensaje(char* mensaje, int socket_cliente);

void* recibir_buffer(int* size, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
int recibir_operacion(int socket_cliente);

//int recv_instruccion(int fd_modulo, char* path, int pc);
t_list* recibir_paquete(int socket_cliente); //esta funcion creo q esta muy mal -nico




#endif
