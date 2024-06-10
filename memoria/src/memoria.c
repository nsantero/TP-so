#include <memoria.h>

void cargar_configuracion(char* archivo_configuracion)
{
	t_config* config = config_create(archivo_configuracion); //Leo el archivo de configuracion

	if (config == NULL) {
		perror("Archivo de configuracion no encontrado");
		exit(-1);
	}

	config_valores.puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    config_valores.tam_memoria = config_get_int_value(config,"TAM_MEMORIA");
    config_valores.tam_pagina = config_get_int_value(config,"TAM_PAGINA");
    config_valores.path_instrucciones = config_get_string_value(config,"PATH_INSTRUCCIONES");
    config_valores.retardo_respuesta = config_get_int_value(config,"RETARDO_RESPUESTA");
	
}

int main(int argc, char *argv[])
{

    logger = log_create("../memoria/src/memoria.log", "MEMORIA", true, LOG_LEVEL_INFO);
	log_info(logger, "Se creo el log!");

	cargar_configuracion("/home/utnso/tp-2024-1c-File-System-Fanatics/memoria/memoria.config");

    lista_procesos_activos = list_create(); 

	int server_fd = iniciar_servidor(logger, server_name ,IP, config_valores.puerto_escucha);  //cambiar variable global
	log_info(logger, "Servidor listo para recibir al cliente");

	// espero a kernel y cpu
	while(server_escuchar(server_fd));
	


    return 0;
}

////////////////////////////////////////////////////////// PROCESO CONEXION //////////////////////////////////////////////////////////

static void procesar_conexion(void *void_args) {
	int *args = (int*) void_args;
	int cliente_socket = *args;
	op_code cop;
    
	while (cliente_socket != -1) {
		if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
			log_info(logger, ANSI_COLOR_BLUE"El cliente se desconecto de %s server", server_name);
			return;
		}
		switch (cop) {
		case MENSAJE:{

			char* mensaje = recibir_mensaje(cliente_socket);
            log_info(logger, ANSI_COLOR_YELLOW"Me llegó el mensaje: %s", mensaje);
                
            free(mensaje); // Liberar la memoria del mensaje recibido

			break;
            }
        case CREAR_PROCESO: {
            //  Recibo el pid y el nombre del archivo
            int *pid;
            char *nombre_archivo;
            //recv_inicio_proceso(cliente_socket,&pid,&nombre_archivo);
            log_info(logger,"nombre archivo: %s", nombre_archivo);
            t_proceso* proceso = malloc(sizeof(t_proceso));
            proceso->pid = *pid;
            proceso->archivos_instrucciones = nombre_archivo;
            
            list_add(lista_procesos_activos,proceso);
            /*
            free(pid);
            free(nombre_archivo);
            */
           break;
        }
		case PEDIDO_INSTRUCCION:{
            
            int *pid;
            int *pc;
            usleep(config_valores.retardo_respuesta *1000);
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
}

int server_escuchar(int fd_memoria) {
	server_name = "Memoria";
	int cliente_socket = esperar_cliente(logger, server_name, fd_memoria);

	if (cliente_socket != -1) {
		pthread_t hilo;
		pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) &cliente_socket);
		pthread_detach(hilo);
		return 1;
	}

	return 0;
}

int recv_inicio_proceso(int cliente_socket,int **pid, char **nombre_archivo){

    t_list *paquete = recibir_paquete(cliente_socket);
	
	*pid = (int *)list_get(paquete, 0);

    *nombre_archivo = (char *)list_get(paquete, 1);

	log_info(logger,"me llego el archivo: %s",*nombre_archivo);

	list_destroy(paquete);
	return 0;

}

int recv_fetch_instruccion(int fd_modulo, int **pid, int **pc)
{
	t_list *paquete = recibir_paquete(fd_modulo);

	// Obtener el path del paquete
	*pid = (int *)list_get(paquete, 0);

	log_info(logger,"me llego el pid: %d",**pid);

	
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
    for(int i = 0; list_size(lista_procesos_activos) > i && encontrado == false ;i++){
        t_proceso* proceso = list_get(lista_procesos_activos,i);
        if(proceso->pid == pid){
            encontrado = true;
            archivo_instruccion = proceso->archivos_instrucciones;

        }
    }

    string_append(&path_completo, archivo_instruccion);
    return path_completo;
}

void leer_instruccion_por_pc_y_enviar(int *pid, int *pc, int fd) {
    log_info(logger,"Recibi: %d",pid);
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
            //log_info(logger,"Instrucción %d: %s", pc, instruccion_leida);
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
