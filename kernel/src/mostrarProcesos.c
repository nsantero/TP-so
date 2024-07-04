#include <mostrarProcesos.h>

void mostrar_procesos_en_lista(t_list* lista, const char* nombre_lista) {
    printf("Procesos en %s:\n", nombre_lista);
    list_iterate(lista, imprimir_elemento_lista);
}
void imprimir_elemento_lista(void* data) {
    PCB* proceso = (PCB*) data;
    imprimir_proceso(proceso);
}
void mostrar_todas_las_listas() {
    pthread_mutex_lock(&mutexListaNew);
    pthread_mutex_lock(&mutexListaReady);
    pthread_mutex_lock(&mutexListaBlocked);
    pthread_mutex_lock(&mutexListaRunning);
    pthread_mutex_lock(&mutexListaExit);
    mostrar_procesos_en_lista(lista_NEW, "ListaNew");
    mostrar_procesos_en_lista(lista_READY, "ListaReady");
    mostrar_procesos_en_lista(lista_BLOCKED, "ListaBlocked");
    mostrar_procesos_en_lista(lista_RUNNING, "ListaRunning");
    mostrar_procesos_en_lista(lista_EXIT, "ListaExit");
    pthread_mutex_unlock(&mutexListaNew);
    pthread_mutex_unlock(&mutexListaReady);
    pthread_mutex_unlock(&mutexListaBlocked);
    pthread_mutex_unlock(&mutexListaRunning);
    pthread_mutex_unlock(&mutexListaExit);
    return;
}
void imprimir_proceso(PCB* proceso) {
    char *estadoLabel[] = {"NEW", "READY", "Running", "BLOCKED", "EXIT"};
    printf("PID: %d, PC: %d, Quantum: %d, Estado: %s\n", proceso->PID, proceso->cpuRegisters.PC, proceso->quantum, estadoLabel[proceso->estado]);

    return;
}