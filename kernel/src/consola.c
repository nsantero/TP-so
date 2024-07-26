#include <consola.h>

void* manejadorDeConsola(){

    char *linea=NULL;
    char *comando=NULL;
    char *segundoArgumento=NULL;

    while(1){
        linea=readline("Kernel> ");
        if(strlen(linea)>0)
        {
            add_history(linea);
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
                    detener_planificacion();
                    crearPCB(segundoArgumento);
                    iniciar_planificacion();
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
                    modificar_grado_multiprogramacion(valor);
                } else {
                    printf("Error: Falta el argumento [VALOR].\n");
                }
            }
            else if (!strcmp(comando, "PROCESO_ESTADO")) {
                mostrar_todas_las_listas();
            }else if (!strcmp(comando, "MATAR")) {
                terminarKernel();
                free(linea);
                return NULL;
            } else {
                printf("Comando no reconocido: %s\n", comando);
            }
        }
        free(linea);
        
    }
    
}
void liberarPCB(PCB* pcb) {
    if (pcb != NULL) {
        // Liberar la lista de recursos en uso
        if (pcb->recursosEnUso != NULL) {
            list_destroy(pcb->recursosEnUso);  // Ajusta el segundo parámetro si necesitas liberar elementos específicos
        }
        free(pcb);
    }
}

// Función para liberar todas las listas y sus elementos
void liberarTodasLasListas() {
    // Definir un array de listas
    t_list* listas[] = { lista_NEW, lista_READY, lista_READYPRI, lista_EXIT, lista_BLOCKED, lista_BLOCKED_RECURSOS, lista_RUNNING };
    int numListas = 7;

    // Recorrer cada lista
    for (int i = 0; i < numListas; i++) {
        t_list* lista = listas[i];
        if (lista != NULL) {
            // Recorrer y liberar cada PCB en la lista
            while (!list_is_empty(lista)) {
                PCB* pcb = list_remove(lista, 0);
                liberarPCB(pcb);
            }
            // Liberar la lista
            list_destroy(lista);
        }
    }
}
void terminarKernel(){
    pthread_mutex_lock(&mutexListaNew);
    pthread_mutex_lock(&mutexListaReady);
    pthread_mutex_lock(&mutexListaReadyPri);
    pthread_mutex_lock(&mutexListaBlocked);
    pthread_mutex_lock(&mutexListaBlockedRecursos);
    pthread_mutex_lock(&mutexListaRunning);
    pthread_mutex_lock(&mutexListaExit);
    liberarTodasLasListas();
    pthread_mutex_unlock(&mutexListaNew);
    pthread_mutex_unlock(&mutexListaReady);
    pthread_mutex_unlock(&mutexListaReadyPri);
    pthread_mutex_unlock(&mutexListaBlocked);
    pthread_mutex_unlock(&mutexListaBlockedRecursos);
    pthread_mutex_unlock(&mutexListaRunning);
    pthread_mutex_unlock(&mutexListaExit);
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
                detener_planificacion();
                crearPCB(fullPath);
                iniciar_planificacion();
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
    pthread_t detener_new, detener_ready, detener_exec, detener_blocked, detener_blocked_recursos, detener_ready_prioridad;
    pthread_create(&detener_new,NULL,detenerListanew,NULL);
    pthread_create(&detener_ready,NULL,detenerListaready,NULL);
    pthread_create(&detener_exec,NULL,detenerListaexec,NULL);
    pthread_create(&detener_blocked,NULL,detenerListablocked,NULL);
    pthread_create(&detener_blocked_recursos,NULL,detenerListaBlockedrecursos,NULL);
    pthread_create(&detener_ready_prioridad,NULL,detenerListaReadyprioridad,NULL);
    pthread_detach(detener_new);
    pthread_detach(detener_ready);
    pthread_detach(detener_exec);
    pthread_detach(detener_blocked);
    pthread_detach(detener_blocked_recursos);
    pthread_detach(detener_ready_prioridad);
}
void* detenerListanew()
{
    sem_wait(&semPlaniNew);
    return NULL;
}
void* detenerListaready()
{
    sem_wait(&semPlaniReady);
    return NULL;
}
void* detenerListaexec()
{
    sem_wait(&semPlaniRunning);
    return NULL;
}
void* detenerListablocked()
{
    sem_wait(&semPlaniBlocked);
    return NULL;
}

void* detenerListaBlockedrecursos()
{
    sem_wait(&semPlaniBlockedRecursos);
    return NULL;
}

void* detenerListaReadyprioridad()
{
    sem_wait(&semPlaniReadyClock);
    return NULL;
}

void iniciar_planificacion() {
    sem_post(&semPlaniNew);
    sem_post(&semPlaniReady);
    sem_post(&semPlaniRunning);
    sem_post(&semPlaniBlocked);
    sem_post(&semPlaniBlockedRecursos);
    sem_post(&semPlaniReadyClock);
}

void modificar_grado_multiprogramacion(int valor) {
    configuracionKernel.GRADO_MULTIPROGRAMACION = valor;
    pthread_mutex_lock(&mutexListaReady);
    for (int i = 0; i < list_size(lista_NEW); i++) {
        sem_post(&semListaNew);
    }
    pthread_mutex_unlock(&mutexListaReady);
    
}

