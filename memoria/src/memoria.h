#ifndef MEMORIA_H
#define MEMORIA_H

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include "../../shared/src/shared.h"
#include "configs.h"

// ESTURCTURAS

// Estructura de la Tabla de Paginas por proces
typedef struct{	
    int pid_tabla_de_paginas;
    int numero_de_pagina;
    int numero_de_frame; 	
    bool modificado;
} Registro_tabla_paginas_proceso;

// Estructura del Proceso
typedef struct {
    int pid;
    char *path;
    t_list *instrucciones;
    int tam_proceso;
    t_list *tabla_de_paginas;
} Proceso;

typedef struct {
    int numeroInstruccion;
    char *instruccion;
} Instruccion;

// Estructura de la memoria
typedef struct{	   
    void* espacioUsuario; 
    int tam;
    int pagina_tam;
    int cantidad_frames;
    t_bitarray* bitmap_frames;
} Memoria;

extern Memoria memoria;

extern t_bitarray* bitmap_frames;

void crearListas();

void esquemaPaginacion();

int calculoDeFrames(int memoria_tam, int pagina_tam);

void inicializarMemoria();

extern t_list* lista_ProcesosActivos;

#endif