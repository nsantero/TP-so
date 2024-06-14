#include <utils.h>

extern int memoria_fd;
extern int cpu_dispatch_fd;
extern int cpu_interrupt_fd;
extern int server_fd;
extern char* server_name;

static void procesar_conexion(void *void_args);
extern int server_escuchar(int fd_memoria);