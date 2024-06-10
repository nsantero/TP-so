#include <shared.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <commons/collections/list.h>
#include <assert.h>
#include <pthread.h>

typedef struct
{		
    char* nombre_interfaz;
    char* tipo_interfaz;
    bool esta_conectada;
    int tiempo_unidad_trabajo;
    char* ip_kernel;
    char* puerto_kernel;
    char* ip_memoria;
    char* puerto_memoria;
    char* path_base_dials;
    int block_size;
    int block_count;
    int retraso_compactacion;

} entradasalida_config;

entradasalida_config config_valores;
t_log* logger;
int kernel_fd;
int memoria_fd;

void cargar_configuracion(char* archivo_configuracion);