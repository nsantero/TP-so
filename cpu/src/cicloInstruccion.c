#include <cicloInstruccion.h>
#include <cpu.h>
#include <utils.h>

void recv_instruccion(int memoria_fd);


void fetch(){

    //mandar mensaje a memoria

    //recibir de memoria y guardar en instruccion
    recv_instruccion(memoria_fd);
}

// Comento la otra opción porque estoy teniendo problemas con el switch
void decode_execute() {
    if (strcmp(instruccionActual.instruccion, "SUM") == 0) {
        // Código para manejar la instrucción SUM
    } else if (strcmp(instruccionActual.instruccion, "SUB") == 0) {
        // Código para manejar la instrucción SUB
    } 
}
/*
void decode_excecute(){

    switch (instruccionActual.instruccion)
    {
    case "SUM":
        // code 
        break;
    
    default:
        break;
    }
}
*/

void checkInterrupt(){
}

void recv_instruccion(int memoria_fd){

	t_list *paquete = recibir_paquete(memoria_fd);

	// Obtener el path del paquete
	char *instruccion_cadena = (char *)list_get(paquete, 0);

	log_info(logger, "Recibi la instruccion: %s", instruccion_cadena ); 

	char **palabras = string_split(instruccion_cadena , " ");
    
    //int tamanio_array = (sizeof(palabras) / sizeof(palabras[0]));

    int tamanio_array = 0;
    while ((palabras)[tamanio_array] != NULL) {
        tamanio_array++;
    }

	instruccionActual.instruccion = malloc(sizeof(char) * (strlen(palabras[1]) + 1));
    strcpy(instruccionActual.instruccion, palabras[0]); 

	instruccionActual.operando1 = malloc(sizeof(char) * (strlen(palabras[1]) + 1));
    strcpy(instruccionActual.operando1, palabras[1]); 

    if(tamanio_array > 2){

        instruccionActual.operando2 = malloc(sizeof(char) * (strlen(palabras[2]) + 1));
        strcpy(instruccionActual.operando2, palabras[2]);
    }

    if(tamanio_array > 3){

        instruccionActual.operando3 = malloc(sizeof(char) * (strlen(palabras[3]) + 1));
        strcpy(instruccionActual.operando3, palabras[3]);
    }

    if(tamanio_array > 4){

        instruccionActual.operando4 = malloc(sizeof(char) * (strlen(palabras[4]) + 1));
        strcpy(instruccionActual.operando4, palabras[4]);
    }

    if(tamanio_array > 5){

        instruccionActual.operando5 = malloc(sizeof(char) * (strlen(palabras[5]) + 1));
        strcpy(instruccionActual.operando5, palabras[5]);
    }
	

        /*

    // Liberar memoria asignada a palabras
    int i = 0;
    while (palabras[i] != NULL) {
        free(palabras[i]);
        i++;
    } */
    free(palabras);
	free(paquete);
    
}
/*
void modificarPCB(PCB* proceso, int new_pid, int new_pc) {
    proceso->PID = new_pid;
    proceso->pc = new_pc;
}*/
/*
void recibir_proceso(int socket_fd) {
    // Crear buffer para recibir datos
    int PID;
    char path[256];
    int pc;

    // Recibir PID
    recv(socket_fd, &PID, sizeof(int), 0);
    // Recibir path
    recv(socket_fd, path, sizeof(path), 0);
    // Recibir PC
    recv(socket_fd, &pc, sizeof(int), 0);

    // Crear PCB
    PCB* proceso = obtener_pcb_por_pid(PID);
    modificarPCB(proceso, PID, pc);

    // Ejecutar el proceso
    fetch();
    decode_execute();
}
*/