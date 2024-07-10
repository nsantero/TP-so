#include <conexiones.h>
#include <semaforos.h>

int server_fd = 0;

void* atenderPeticionesKernel() {
    while (1) {
        int socketCliente = esperarClienteV2(loggerMemoria, server_fd);
        pthread_t client_thread;
        int* pclient = malloc(sizeof(int));
        *pclient = socketCliente;
        pthread_create(&client_thread, NULL, manejarClienteKernel, pclient);
        pthread_detach(client_thread);
    }
    return NULL;
}

void* manejarClienteKernel(void *arg)
{
    int socketCliente = *((int*)arg);
    free(arg);
    while(1){
        pthread_mutex_lock(&mutexSocketKernel);
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

        
        recv(socketCliente, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(socketCliente, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);

        switch(paquete->codigo_operacion){
            case CREAR_PROCESO:
            {
                Proceso *proceso = malloc(sizeof(Proceso));

                void *stream = paquete->buffer->stream;
                int pathLenght;

                memcpy(&proceso->pid, stream, sizeof(int));
                stream += sizeof(int);
                memcpy(&pathLenght, stream, sizeof(int));
                stream += sizeof(int);
                proceso->path = malloc(pathLenght);
                memcpy(proceso->path, stream, pathLenght);
                cargarInstrucciones(proceso, proceso->path);
                //crear lista con las instrucciones
                //aniadir a lista de procesos
                //destruir paquete
                //ESPERAR MILISEGUNDOS
                printf("\n");
                char *primeraInstruccion = list_get(proceso->instrucciones, 0);
                printf("PRIMERA INSTRUCCION:%s\n", primeraInstruccion);
                break;
            }
            case FINALIZAR_PROCESO:
            {
                //IMPLEMENTAR
                break;
            }
            default:
            {   
                log_error(loggerMemoria, "Se recibio un operacion de kernel NO valido");
                break;
            }
        }
        pthread_mutex_unlock(&mutexSocketKernel);
    }
    close(server_fd);
    close(socketCliente);
}

void cargarInstrucciones(Proceso *proceso, const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    proceso->instrucciones = list_create();

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;

        Instruccion *instruccion = malloc(sizeof(Instruccion));
        instruccion->instruccion = strdup(line);

        printf("instruccion:%s\n", instruccion->instruccion);

        list_add(proceso->instrucciones, instruccion->instruccion);
    }

    fclose(file);
}