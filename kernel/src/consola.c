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
                paquete_memoria_finalizar_proceso(pid);
                //finalizar_proceso(pid);
            } else {
                printf("Error: Falta el argumento [PID].\n");
            }
        }
        else if (!strcmp(comando, "DETENER_PLANIFICACION")) {
            //detener_planificacion();
        }
        else if (!strcmp(comando, "INICIAR_PLANIFICACION")) {
            //iniciar_planificacion();
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

    char* linea = NULL;
    size_t len = 0;
    ssize_t read;
    
    while ((read = getline(&linea, &len, archivo)) != -1) {
        // Eliminar el carácter de nueva línea si existe
        if (linea[read - 1] == '\n') {
            linea[read - 1] = '\0';
        }
        procesarLinea(linea);
    }

    free(linea);
    fclose(archivo);
}

void procesarLinea(char* linea) {
    char* comando = strtok(linea, " ");
    char* argumento = strtok(NULL, " ");
    
    if (comando == NULL) {
        return;
    }
    
    if (strcmp(comando, "INICIAR_PROCESO") == 0) {
        if (argumento != NULL) {
            crearPCB(argumento);
        } else {
           //log_error("Error: Falta el argumento [PATH] para INICIAR_PROCESO\n");
        }
    }
}