#ifndef KERNEL_H
#define KERNEL_H

#include <utils.h>
#include <pcb.h>

void inicializarListas();
// LISTAS DE ESTADOS

    extern t_list* lista_NEW;
    extern t_list* lista_READY; 
    extern t_list* lista_EXIT;
    extern t_list* lista_BLOCKED;
    extern t_list* lista_RUNNING;

// PCB

extern int quantum ; 
extern char* algoritmo_planificacion;
extern int pidActual;
extern int generarPID();
extern int obtener_pid(void);

// Funciones para planificar

extern int leer_grado_multiprogramación();
extern int pid_counter;
// envios a cpu

void paquete_crear_proceso(int PID_paquete, char* path_paquete, int pc_paquete);


#endif