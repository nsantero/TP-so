#include <consola.h>

void* manejadorDeConsola(){

    char *linea=NULL;
    char *comando=NULL;
    char *segundoArgumento=NULL;

    while(1){
        linea=readline("Kernel> ");
        if(linea)
        {
            add_history(linea);
        }
        comando = strtok(linea, " ");
        //TODO estaria bueno q no rompa si el comando q le pasan esta mal, q mande un comando erroneo o algo asi
        if(!strcmp(comando,"EJECUTAR_SCRIPT")){
            segundoArgumento = strtok(NULL, " ");
            if(segundoArgumento){
                ejecutarScript(segundoArgumento);
            }
            else{
                printf("Error: Falta el argumento [PATH]\n");
            }
        }
        else if(!strcmp(comando, "INICIAR_PROCESO")){
            segundoArgumento = strtok(NULL, " ");
            if (segundoArgumento) {
                //iniciar_proceso(segundoArgumento);
                crearPCB(segundoArgumento);
            } else {
                printf("Error: Falta el argumento [PATH].\n");
            }
        }
        else if (!strcmp(comando, "FINALIZAR_PROCESO")) {
            segundoArgumento = strtok(NULL, " ");
            if (segundoArgumento) {
                int pid = atoi(segundoArgumento);
                printf("PID %d \n", pid);
                detener_planificacion();
                finalizarProceso((uint32_t)pid);
                iniciar_planificacion();
            } else {
                printf("Error: Falta el argumento [PID].\n");
            }
        }
        else if (!strcmp(comando, "DETENER_PLANIFICACION")) {
            detener_planificacion();

        }
        else if (!strcmp(comando, "INICIAR_PLANIFICACION")) {
            iniciar_planificacion();
        }
        else if (!strcmp(comando, "MULTIPROGRAMACION")) {
            segundoArgumento = strtok(NULL, " ");
            if (segundoArgumento) {
                int valor = atoi(segundoArgumento);
                //modificar_multiprogramacion(valor);
            } else {
                printf("Error: Falta el argumento [VALOR].\n");
            }
        }
        else if (!strcmp(comando, "PROCESO_ESTADO")) {
            mostrar_todas_las_listas();
        } else {
            printf("Comando no reconocido: %s\n", comando);
        }
        free(linea);
    }
    
}

void ejecutarScript(char* path){
    FILE* archivo = fopen(path, "r");
    if (archivo == NULL) {
        //log_error("Error: No se puede abrir el archivo %s\n", path);
        return;
    }
    char* lastSlash = strrchr(path, '/');
    char scriptBasePath[1024];
    if (lastSlash != NULL) {
        size_t basePathLength = lastSlash - path;
        strncpy(scriptBasePath, path, basePathLength);
        scriptBasePath[basePathLength] = '\0'; // Agregar terminador nulo
    } else {
        strcpy(scriptBasePath, ".");
    }


    char* linea = NULL;
    size_t len = 0;
    ssize_t read;
    
    while ((read = getline(&linea, &len, archivo)) != -1) {
        // Eliminar el carácter de nueva línea si existe
        if (linea[read - 1] == '\n') {
            linea[read - 1] = '\0';
        }
        procesarLinea(linea, scriptBasePath);
    }

    free(linea);
    fclose(archivo);
}

void procesarLinea(char* linea,  const char* scriptBasePath) {
    char* comando = strtok(linea, " ");
    char* argumento = strtok(NULL, " ");
    
    if (comando == NULL) {
        return;
    }
    
    if (strcmp(comando, "INICIAR_PROCESO") == 0) {
        if (argumento != NULL) {
            char* fullPath = construirPathCompleto(argumento);
            if (fullPath != NULL) {
                crearPCB(fullPath);
                free(fullPath);
            };
        } else {
           //log_error("Error: Falta el argumento [PATH] para INICIAR_PROCESO\n");
        }
    }
}
char* construirPathCompleto(char* argumento) {
    char prefijo[] = "/home/utnso/Documents/c-comenta-pruebas";
    size_t size = strlen(prefijo) + strlen(argumento) + 1;
    char* nuevo_path = (char*)malloc(size);

    if (nuevo_path == NULL) {
        fprintf(stderr, "Error al asignar memoria\n");
        return NULL;
    }

    strcpy(nuevo_path, prefijo);
    strcat(nuevo_path, argumento);

    return nuevo_path;
}

void detener_planificacion() {
    pthread_t detener_new, detener_ready, detener_exec, detener_blocked;
    pthread_create(&detener_new,NULL,(void*) detener_cola_new,NULL);
    pthread_create(&detener_ready,NULL,(void*) detener_cola_ready,NULL);
    pthread_create(&detener_exec,NULL,(void*) detener_cola_exec,NULL);
    pthread_create(&detener_blocked,NULL,(void*) detener_cola_blocked,NULL);
    pthread_detach(detener_new);
    pthread_detach(detener_ready);
    pthread_detach(detener_exec);
    pthread_detach(detener_blocked);
}
void detener_cola_new(void* arg)
{
    sem_wait(&semPlaniNew);
}
void detener_cola_ready(void* arg)
{
    sem_wait(&semPlaniReady);
}
void detener_cola_exec(void* arg)
{
    sem_wait(&semPlaniRunning);
}
void detener_cola_blocked(void* arg)
{
    sem_wait(&semPlaniBlocked);
}

void iniciar_planificacion() {
    sem_post(&semPlaniNew);
    sem_post(&semPlaniReady);
    sem_post(&semPlaniRunning);
    sem_post(&semPlaniBlocked);
}