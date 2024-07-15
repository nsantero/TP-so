#include "utils.h"

int memoria_fd;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
char* server_name_dispatch = "CPU_DISPATCH";
char* server_name_interrupt = "CPU_INTERRUPT";
t_list* listaPCBS;
t_instruccion instruccionActual;
extern int interrumpir;
op_code codigo_op_hilo_interrupt_cpu;

void* escucharInterrupciones(){

    codigo_op_hilo_interrupt_cpu = recibir_operacion(fd_cpu_interrupt);

    switch (codigo_op_hilo_interrupt_cpu)
    {
    case PAUSAR_EJECUCION:
        /**
         * Aca hay un tema q para mi se puede solucionar
         * con semaforos, pero lo quiero preguntar el sabado
         * TODO
        */
        break;
    //case INTERRUMPIR_PROCESO_ACTUAL:
    //    interrumpir=1;
    //    break;
    
    default:
        break;
    }
    return NULL;
}