#include <memoria.h>
#include <conexiones.h>
#include <semaforos.h>

Memoria memoria;
t_list *lista_frames;
t_list *lista_de_paginas_proceso;
t_list *lista_ProcesosActivos=NULL;

void crearListas(){

    lista_ProcesosActivos = list_create(); 
    lista_frames = list_create(); 
    lista_de_paginas_proceso = list_create(); 

}

void esquemaPaginacion(){
    
    int calculo_espacio_frames = memoria.cantidad_frames/8 +1 ;
    
    void* espacio_frames =malloc(calculo_espacio_frames);
     
    t_bitarray* bitmap_frames =  bitarray_create_with_mode(espacio_frames,calculo_espacio_frames,LSB_FIRST);

}

int calculoDeFrames(int memoria_tam, int pagina_tam){

    return memoria_tam/pagina_tam;

}

void inicializarMemoria(){

    memoria.tam = configuracionMemoria.TAM_MEMORIA;
    memoria.pagina_tam = configuracionMemoria.TAM_PAGINA;
    memoria.cantidad_frames = calculoDeFrames(memoria.tam, memoria.pagina_tam);
    memoria.espacioUsuario = malloc(memoria.tam);

}


int main(int argc, char *argv[])
{
    
    printf("Modulo Memoria\n");

    iniciarLoggerMemoria();

	armarConfigMemoria();

    crearListas();

    inicializarMemoria();

    esquemaPaginacion();
    
    
	//int server_fd = iniciar_servidor(loggerMemoria, server_name ,IP, configuracionMemoria.PUERTO_ESCUCHA );  //cambiar variable global
    server_fd = iniciarServidorV2(loggerMemoria, configuracionMemoria.PUERTO_ESCUCHA);
    
	log_info(loggerMemoria, "Servidor listo para recibir al cliente");
    pthread_t hiloCpu;
    pthread_create(&hiloCpu, NULL, atenderPeticionesCpu, NULL);
    pthread_t hiloKernel;
    pthread_create(&hiloKernel, NULL, atenderPeticionesKernel, NULL);

    pthread_detach(hiloCpu);
    pthread_detach(hiloKernel);


    while(1){


    }
    
    close(server_fd);
    return 0;
}


////////////////////////////////////////////////////////// PROCESO CONEXION //////////////////////////////////////////////////////////

/*static void procesar_conexion(void *void_args) {
	int *args = (int*) void_args;
	int cliente_socket = *args;
	op_code cop;
    
	while (cliente_socket != -1) {
		if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
			log_info(loggerMemoria, ANSI_COLOR_BLUE"El cliente se desconecto de %s server", server_name);
			return;
		}
		switch (cop) {
		case MENSAJE:{

			char* mensaje = recibir_mensaje(cliente_socket);
            log_info(loggerMemoria, ANSI_COLOR_YELLOW"Me llegó el mensaje: %s", mensaje);
                
            free(mensaje); // Liberar la memoria del mensaje recibido

			break;
            }
        case CREAR_PROCESO: {
            //  Recibo el pid y el nombre del archivo
            int *pid;
            char *nombre_archivo;
            //recv_inicio_proceso(cliente_socket,&pid,&nombre_archivo);
            log_info(loggerMemoria,"nombre archivo: %s", nombre_archivo);
            t_proceso* proceso = malloc(sizeof(t_proceso));
            proceso->pid = *pid;
            proceso->archivos_instrucciones = nombre_archivo;
            
            list_add(lista_ProcesosActivos,proceso);
            
            free(pid);
            free(nombre_archivo);
           break;
        }
		case PEDIDO_INSTRUCCION:{
            
            int *pid;
            int *pc;
            usleep(configuracionMemoria.RETARDO_RESPUESTA *1000);
            recv_fetch_instruccion(cliente_socket, &pid,&pc);
            //leer_instruccion_por_pc_y_enviar(*pid,*pc, cliente_socket);
            free(pid); 
            free(pc); 
            
            break;
            } 

		case PAQUETE:

			///// IMPLEMENTAR
			
			break;
		
        default:
            printf("Error al recibir  %d \n", cop);
            break;
	return;
        }
    }
}*/

/*int server_escuchar(int fd_memoria) {
	server_name = "Memoria";
	int cliente_socket = esperar_cliente(loggerMemoria, server_name, fd_memoria);

	if (cliente_socket != -1) {
		pthread_t hilo;
		pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) &cliente_socket);
		pthread_detach(hilo);
		return 1;
	}

	return 0;
}*/

int recv_inicio_proceso(int cliente_socket,int **pid, char **nombre_archivo){

    t_list *paquete = recibir_paquete(cliente_socket);
	
	*pid = (int *)list_get(paquete, 0);

    *nombre_archivo = (char *)list_get(paquete, 1);

	log_info(loggerMemoria,"me llego el archivo: %s",*nombre_archivo);

	list_destroy(paquete);
	return 0;

}

int recv_fetch_instruccion(int fd_modulo, int **pid, int **pc)
{
	t_list *paquete = recibir_paquete(fd_modulo);

	// Obtener el path del paquete
	*pid = (int *)list_get(paquete, 0);

	log_info(loggerMemoria,"me llego el pid: %d",**pid);

	
	*pc = (int *)list_get(paquete, 1);

	list_destroy(paquete);
	return 0;
}

/*char *armar_path_instruccion(int pid) {
    char *path_completo = string_new();
    string_append(&path_completo, config_valores.path_instrucciones);
    string_append(&path_completo, "/");
    
    char* archivo_instruccion;

    bool encontrado = false; 
    for(int i = 0; list_size(lista_ProcesosActivos) > i && encontrado == false ;i++){
        t_proceso* proceso = list_get(lista_ProcesosActivos,i);
        if(proceso->pid == pid){
            encontrado = true;
            archivo_instruccion = proceso->archivos_instrucciones;

        }
    }

    string_append(&path_completo, archivo_instruccion);
    return path_completo;
}

void leer_instruccion_por_pc_y_enviar(int *pid, int *pc, int fd) {
    log_info(loggerMemoria,"Recibi: %d",pid);
    char *path_completa_instruccion = armar_path_instruccion(pid);

    FILE *archivo = fopen(path_completa_instruccion, "r");
    if (archivo == NULL) {
        perror("No se pudo abrir el archivo de instrucciones");
        free(path_completa_instruccion);
        return;
    }

    char instruccion_leida[256];
    int current_pc = 0;

    while (fgets(instruccion_leida, sizeof(instruccion_leida), archivo) != NULL) {
        if (current_pc == pc) {
            //log_info(loggerMemoria,"Instrucción %d: %s", pc, instruccion_leida);
            char* instruccion = instruccion_leida;
            


            t_paquete *paquete = crear_paquete(PEDIDO_INSTRUCCION);
			agregar_a_paquete(paquete, instruccion, strlen(instruccion) + 1);
			enviar_paquete(paquete, fd);
			eliminar_paquete(paquete);

            // Liberar memoria asignada para la estructura Instruccion


            //free(instruccion);
            
            
            break;
        }
        current_pc++;
    }
    fclose(archivo);
    free(path_completa_instruccion);
}*/
