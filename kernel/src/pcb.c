#include <pcb.h>
#include <stdio.h>

int pid_counter = 1;
pthread_mutex_t mutex_pid;

int generarPID(void) {
    pthread_mutex_lock(&mutex_pid);
    int pid = pid_counter++;
    pthread_mutex_unlock(&mutex_pid);
    return pid;
}

PCB* crearPCB(char* path) {
    printf("Creando PCB...\n");

    PCB* nuevoPCB = malloc(sizeof(PCB));
    nuevoPCB->pid = generarPID();
    nuevoPCB->pc = 0;
    nuevoPCB->Estado = NEW; // Estado inicial
    nuevoPCB->quantum = 0; // Establecer un valor por defecto para quantum

    return nuevoPCB;
}
