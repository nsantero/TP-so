#include <utils.h>
#include <kernel.h>
#include <conexiones.h>


int main(int argc, char *argv[]) {
    

    cargar_configuracion(archivo_configuracion);
    inicializarListas();
    while (1)
    {
        linea = readline(">");

        if (linea)
        {
            add_history(linea);
        }
// PASO 1 - Se ejecutarán los scripts indicados en las pruebas 
        
        if (!strncmp(linea, "EJES ", 5)) // EJECUTAR_SCRIPT
        { // ejecutar script de comandos
            path_script = strdup(linea + strlen("EJECUTAR_SCRIPT "));
            printf("Se esta ejecutando el script del siguiente path: %s\n", path_script);
            
            PCB* recibido = crearPCB();
            printf("Se creo un nuevo proceso: %d\n", recibido->PID);
            planificar_largo_plazo(recibido, path_script);
            printf("Se planifico largo plazo: %d\n", recibido->PID);
            // cpu el PCB (PID y PC) -- PAQUETES
            // memoria el PCB (Pseudocodigo, PID) -- PAQUETES
            // signal
            // 
            //free(path_script);
            free(linea);
        }

        if (!strncmp(linea, "INICIAR_PROCESO ", 16))
        {
            free(path_proceso);
            path_proceso = strdup(linea + strlen("INICIAR_PROCESO "));

            printf("Se esta ejecutando el proceso: %s\n", path_proceso);
            free(path_proceso);
            free(linea);

            // deberiamos agregar la función de crearPCB en este caso
        }

        if (!strncmp(linea, "FINALIZAR_PROCESO ", 18))
        {
            free(pid);
            pid = strdup(linea + strlen("FINALIZAR_PROCESO "));

            printf("Se finalizo el proceso: %s\n", pid);
            free(pid);
            free(linea);

            // acá habría que poner remove a la lista de procesos en ejecución, para que se libere y pase el siguiente.
        }

        if (!strncmp(linea, "DETENER_PLANIFICACION", 21))
        {
            printf("%s\n", "Se detuvo la planificacion");
            free(linea);
        }

        if (!strncmp(linea, "INICIAR_PLANIFICACION", 21))
        {
            // acá no deberíamos poner un if para validar que la planificación esta detenida?
            printf("%s\n", "Se inicia la planificacion");
            free(linea);
        }

        if (!strncmp(linea, "MULTIPROGRAMACION ", 18))
        {
            free(valor_multiprogramacion);
            valor_multiprogramacion = strdup(linea + strlen("MULTIPROGRAMACION "));

            printf("Se cambio el valor de multiprogramacion a: %s\n", valor_multiprogramacion);
            free(valor_multiprogramacion);
            free(linea);
        }

        if (!strncmp(linea, "PROCESO_ESTADO", 14))
        {
            printf("%s\n", "Se listan los procesos por estado: ");
            free(linea);
        }

        if (!strncmp(linea, "exit", 4))
        {
            free(linea);
            break;
        }

        // printf("%s\n", linea);
        // free(linea);
    }
    return 0;
}
