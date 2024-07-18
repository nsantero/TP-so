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
int existeInterfaz(char *nombre,Tipos_Interfaz* tipo);
void* manejarClienteIO(void *arg);
void* atenderPeticionesIO();

void paquete_CPU_ejecutar_proceso(PCB* proceso);
void paquete_CPU_interrumpir_proceso_fin_quantum(int pid);
void InterruptACPU();
PCB *recibirProcesoContextoEjecucion(void *stream);
PCB* procesoBloqueado(uint32_t pid);



typedef struct{
    char* nombre;
    int socketCliente;
    Tipos_Interfaz tipo;
}Interfaces_conectadas_kernel;

typedef struct {
    char *nombre;
    int instancias;
    sem_t mutex;
    sem_t waitQueue;
} Recurso;

#endif // CONEXIONES_H