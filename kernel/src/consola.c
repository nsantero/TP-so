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

        if(!strcmp(comando,"EJECUTAR_SCRIPT")){
            segundoArgumento = strtok(NULL, " ");
            if(segundoArgumento){
                //ejecutarScript(path);
            }
            else{
                printf("Error: Falta el argumento [PATH]\n");
            }
        }
        else if(!strcmp(comando, "INICIAR_PROCESO")){
            segundoArgumento = strtok(NULL, " ");
            if (segundoArgumento) {
                //iniciar_proceso(segundoArgumento);
                crearPCB();
            } else {
                printf("Error: Falta el argumento [PATH].\n");
            }
        }
        else if (!strcmp(comando, "FINALIZAR_PROCESO")) {
            segundoArgumento = strtok(NULL, " ");
            if (segundoArgumento) {
                int pid = atoi(segundoArgumento);
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
            //listar_procesos_estado();
            mostrar_todas_las_listas();
        } else {
            printf("Comando no reconocido: %s\n", comando);
        }
        free(linea);
    }
    
}