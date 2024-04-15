#ifndef UTILS_H_
#define UTILS_H_


#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <string.h>
#include <stdlib.h>


/**
* @fn    decir_hola
* @brief Imprime un saludo al nombre que se pase por parámetro por consola.
*/
void decir_hola(char* quien);
int iniciar_servidor(t_log* logger,  char* ip, char* puerto) ;
int crear_conexion(t_log* logger, char* ip, char* puerto) ;
int server_escuchar(t_log* logger, char* server_name, int server_socket) ;


#endif
