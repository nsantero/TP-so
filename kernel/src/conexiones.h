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

void paquete_CPU_ejecutar_proceso(PCB* proceso);
void paquete_CPU_interrumpir_proceso_fin_quantum(int pid);
void InterruptACPU();
PCB *recibirProcesoContextoEjecucion(void *stream);

extern t_list *interfacesConectadas;
extern t_list *interfacesGenericasCola;
extern t_list *interfacesStdinCola;
extern t_list *interfacesStdoutCola;
extern t_list *interfacesDFSCola;
typedef struct{
    char* nombre;
    Tipos_Interfaz tipo;
}Interfaces_conectadas_kernel;

typedef struct {
    char *nombre;
    int instancias;
    sem_t mutex;
    sem_t waitQueue;
} Recurso;

void wait_recurso(char *nombre_recurso, int pid);
void signal_recurso(char *nombre_recurso, int pid);

#endif // CONEXIONES_H