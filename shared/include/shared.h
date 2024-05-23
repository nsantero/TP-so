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
# define IP	"127.0.0.1"

#define ANSI_COLOR_GREEN   "\x1b[32m" // Código de escape para color verde
#define ANSI_COLOR_BLUE    "\x1b[34m" // Código de escape para color azul
#define ANSI_COLOR_YELLOW  "\x1b[33m" // Código de escape para color amarillo
#define ANSI_COLOR_RESET   "\x1b[0m"  // Código de escape para resetear el color
#define ANSI_COLOR_GRAY    "\x1b[90m"  // Código de escape para color gris
#define ANSI_COLOR_CYAN    "\x1b[36m"  // Código de escape para color celeste
#define ANSI_COLOR_PINK    "\x1b[95m"  // Código de escape para color rosa pastel
#define ANSI_COLOR_LIME    "\x1b[92m"


typedef enum
{
    PAQUETE,
    MENSAJE

} op_code;

typedef struct
{
    int size;
    void *stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer *buffer;
} t_paquete;

typedef struct { 
    uint32_t AX, BX, CX, DX; 
}registros_cpu;

typedef struct
{
    int pid;
	int programCounter;
	int quantum;
	registros_cpu* registros_cpu;
} pcb;

/**
* @fn    decir_hola
* @brief Imprime un saludo al nombre que se pase por parámetro por consola.
*/


void decir_hola(char* quien);
int iniciar_servidor(t_log *logger,char* nombre, char *ip, char* puerto);
void* recibir_buffer(int* size, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
t_buffer* crear_buffer(void);
uint8_t stream_recv_de_header(int fromSocket);
int crear_conexion(t_log *logger, const char *server_name, char *ip, char *puerto);
void liberar_conexion(int socket_cliente);
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);
void enviar_mensaje(char* mensaje, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);
void eliminar_paquete(t_paquete* paquete);

#endif
