#ifndef CONEXIONES_H
#define CONEXIONES_H

#include <utils.h>
#include <planificadores.h>
#include <kernel.h>
#include <../../entradasalida/src/peticionesEstructuras.h>

extern int memoria_fd;
extern int cpu_dispatch_fd;
extern int cpu_interrupt_fd;
extern int server_fd;
extern char* server_name;

void* conexionesDispatch();
void inicializar_servidor();
//static void procesar_conexion(void *void_args);
extern int server_escuchar(int fd_memoria);
int existeInterfaz(char *nombre);
void* manejarClienteIO(void *arg);
void* atenderPeticionesIO();

extern t_list *interfacesConectadas;
extern t_list *interfacesGenericasCola;
extern t_list *interfacesStdinCola;
extern t_list *interfacesStdoutCola;
extern t_list *interfacesDFSCola;
typedef struct{
    char* nombre;
    Tipos_Interfaz tipo;
    int ocupada;
    int solicitudesEnCola;
    int pidActual; //nose si es necesario
}Interfaces_conectadas_kernel;

#endif // CONEXIONES_H