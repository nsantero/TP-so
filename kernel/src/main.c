#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include "kernel.c"
#include <commons/config.h>
#include <commons/collections/list.h> //implementación de listas enlazadas que se pueden usar para la cola de procesos



int main(int argc, char* argv[]) {
    char *linea;
    char *path_script = NULL;
    char *path_proceso = NULL;
    char *pid = NULL;
    char *valor_multiprogramacion = NULL;

    while (1) {
        linea = readline(">");

        if (linea) {
            add_history(linea);
        }

        if (!strncmp(linea, "EJECUTAR_SCRIPT ", 16)) { //ejecutar script de comandos
            free(path_script);
            path_script = strdup(linea + strlen("EJECUTAR_SCRIPT "));
            printf("Se esta ejecutando el script del siguiente path: %s\n", path_script);
            free(path_script);
            free(linea);
        }

        if (!strncmp(linea, "INICIAR_PROCESO ", 16)) { 
            free(path_proceso);
            path_proceso = strdup(linea + strlen("INICIAR_PROCESO "));
            
            printf("Se esta ejecutando el proceso: %s\n", path_proceso);
            free(path_proceso);
            free(linea);
        }

        if (!strncmp(linea, "FINALIZAR_PROCESO ", 18)) { 
            free(pid);
            pid = strdup(linea + strlen("FINALIZAR_PROCESO "));
            
            printf("Se finalizo el proceso: %s\n", pid);
            free(pid);
            free(linea);
        }

        if (!strncmp(linea, "DETENER_PLANIFICACION", 21)) { 
            printf("%s\n", "Se detuvo la planificacion");
            free(linea);
        }

        if (!strncmp(linea, "INICIAR_PLANIFICACION", 21)) { 
            printf("%s\n", "Se inicia la planificacion");
            free(linea);
        }
        
        if (!strncmp(linea, "MULTIPROGRAMACION ", 18)) { 
            free(valor_multiprogramacion);
            valor_multiprogramacion = strdup(linea + strlen("MULTIPROGRAMACION "));
            
            printf("Se cambio el valor de multiprogramacion a: %s\n", valor_multiprogramacion);
            free(valor_multiprogramacion);
            free(linea);
        }

        if (!strncmp(linea, "PROCESO_ESTADO", 14)) { 
            printf("%s\n", "Se listan los procesoS por estado: ");
            free(linea);
        }
        
        if (!strncmp(linea, "exit", 4)) {
            free(linea);
            break;
        }

        //printf("%s\n", linea);
        //free(linea);

        
    }
    return 0;
}

