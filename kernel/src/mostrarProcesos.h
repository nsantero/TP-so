#ifndef MOSTRARPROCESOS_H
#define MOSTRARPROCESOS_H
#include "kernel.h"
#include "semaforos.h"
#include "commons/collections/list.h"
void mostrar_procesos_en_lista(t_list* lista, const char* nombre_lista);
void mostrar_todas_las_listas();
void imprimir_proceso(PCB* proceso);
void imprimir_elemento_lista(void* data);

#endif