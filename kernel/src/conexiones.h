#include <utils.h>

typedef struct

{		
    char* puerto_escucha; 
    char* ip_memoria;
    char* puerto_memoria; 
    char* ip_cpu;
    char* puerto_cpu_dispatch; 		
    char* puerto_cpu_interrupt; 				
    char* algoritmo_planificacion; 	
    int quantum;
    char** recursos;
    char** instancias_recursos;
    int grado_multiprogramacion;

} kernel_config;

extern kernel_config config_valores;
extern t_log* logger;
extern int memoria_fd;
extern int cpu_dispatch_fd;
extern int cpu_interrupt_fd;
extern int server_fd;
extern char* server_name;

extern void cargar_configuracion(char* archivo_configuracion);
static void procesar_conexion(void *void_args);
extern int server_escuchar(int fd_memoria);