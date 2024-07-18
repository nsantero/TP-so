#include <cicloInstruccion.h>
#include <stdio.h> 
#include <math.h>

extern int memoria_fd; 
char *instruccionRecibida;
extern int program_counter; 
//t_instruccion instruccion;
char memoria[MEM_SIZE][20];
int interrumpir = 0;
// Memoria ficticia para almacenar instrucciones
 // Cada instrucción tiene un tamaño máximo de 20 caracteres

void* ciclo_de_instruccion() {
    char *instruccion_a_decodificar;
    int valor= 1;
    t_instruccion instruccion;

    while (valor) {

        instruccion_a_decodificar = fetch(procesoEjecutando);
        printf("Instrucción recibida: %s\n", instruccion_a_decodificar);

        //char **cadena_instruccion = malloc(sizeof(char**));
        char **cadena_instruccion = string_split(instruccion_a_decodificar , " ");
        
        if (strstr(cadena_instruccion[0], "EXIT") != NULL ){

            valor =0;
            mandarPaqueteaKernel(PROCESO_EXIT);
            int tamanio_array = 0;
            while ((cadena_instruccion)[tamanio_array] != NULL) {
                free(cadena_instruccion[tamanio_array]);
                tamanio_array++;
            }
            free(cadena_instruccion);
            free(instruccion_a_decodificar);

            //return NULL;
            break;
            
        }

        instruccion = decode(instruccion_a_decodificar,procesoEjecutando->PID);

        int bloqueado = execute2(instruccion,procesoEjecutando->PID);

        if(bloqueado == 1){
            return;
        }
        
        //mutex interrumpir
        if(interrumpir == 1){
            mandarPaqueteaKernel(PROCESO_INTERRUMPIDO_CLOCK);

            int tamanio_array = 0;
            while ((cadena_instruccion)[tamanio_array] != NULL) {
                free(cadena_instruccion[tamanio_array]);
                tamanio_array++;
            }
            free(cadena_instruccion);
            free(instruccion_a_decodificar);

            //return NULL;
            break;
        }

        int tamanio_array = 0;
        while ((cadena_instruccion)[tamanio_array] != NULL) {
            free(cadena_instruccion[tamanio_array]);
            tamanio_array++;
        }
        free(cadena_instruccion);
        free(instruccion_a_decodificar);


    }

    return NULL;
}


char* fetch(Proceso *procesoEjecutando) {
    // Obtener la instrucción de la memoria usando el PC
    // Actualizar el PC para la siguiente instrucción

    paquete_memoria_pedido_instruccion(procesoEjecutando->PID,procesoEjecutando->cpuRegisters.PC);
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    recv(memoria_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
    recv(memoria_fd, &(paquete->buffer->size), sizeof(int), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(memoria_fd, paquete->buffer->stream, paquete->buffer->size, 0);
 

    switch(paquete->codigo_operacion){
            case ENVIO_INSTRUCCION:
            {
                void *stream = paquete->buffer->stream;
                int instruccionLength;

                uint32_t incrementalPC = procesoEjecutando->cpuRegisters.PC +1;
                char *instruccionRecibida;
            
                memcpy(&instruccionLength, stream, sizeof(int));
                stream += sizeof(int);
                instruccionRecibida = malloc(instruccionLength);
                memcpy(instruccionRecibida, stream, instruccionLength);
                procesoEjecutando->cpuRegisters.PC= incrementalPC;
                return instruccionRecibida;
            }
            default:
            {   
                log_error(loggerCpu, "Error");
                break;
            }
    }       
    return NULL;

}


void recibir_confirmacion_memoria_resize(){

    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));

    recv(memoria_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
    recv(memoria_fd, &(paquete->buffer->size), sizeof(int), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(memoria_fd, paquete->buffer->stream, paquete->buffer->size, 0);

    switch(paquete->codigo_operacion){
            case OK:
            {
                printf("Instrucción resize realizada!! \n");
                break;
            }
            case OUT_OF_MEMORY:
            {
                //enviar a kernel 
                printf("Instrucción resize: OUT OF MEMORYY ! \n");
                break;
            }
            default:
            {   
                log_error(loggerCpu, "Error");
                break;
            }

    }       

}


int buscar_frame(int pagina){

    //buscar en tlb

    //buscar en memoria

    return 1;
}

direccion_fisica *traduccion_mmu(char *datos,char *dl, int pid){

    direccion_fisica *direccion = malloc(sizeof(direccion_fisica));

    int direccion_logica = atoi(dl);
    int nro_pagina;

    nro_pagina = floor(direccion_logica / tam_pagina); 

    direccion->PID = pid;

    //logica de paginas divido el tamaño de pagina

    // buscar en memoria el frame y en tlb

    direccion->numero_frame = buscar_frame(nro_pagina);

    direccion->desplazamiento = direccion_logica - nro_pagina * tam_pagina;

    return direccion;
}


void utilizacion_memoria(t_instruccion instruccion_memoria,int pid){

        //DL
        
        
        if(instruccion_memoria.tipo_instruccion = MOV_IN){

            //MOV_IN (Registro Datos, Registro Dirección): 
            //Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el Registro Dirección y lo almacena en el Registro Datos.
            
            direccion_fisica *direccion_fisica = malloc(sizeof(direccion_fisica));
            char* registro_datos_leer = instruccion_memoria.operando1;
            char* registro_direccion_dl = instruccion_memoria.operando2;
            direccion_fisica = traduccion_mmu(registro_direccion_dl,registro_datos_leer,pid);

        }

        if(instruccion_memoria.tipo_instruccion = MOV_OUT){
            //MOV_OUT (Registro Dirección, Registro Datos): 
            //Lee el valor del Registro Datos y 
            //lo escribe en la dirección física de memoria obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
    
            direccion_fisica *direccion_fisica = malloc(sizeof(direccion_fisica));
            char* registro_direccion = instruccion_memoria.operando1;
            char* registro_datos = instruccion_memoria.operando2;
            direccion_fisica = traduccion_mmu(registro_datos,registro_direccion,pid);
        }

        //traduccion_mmu(t_instruccion instruccion_memoria);

}

t_instruccion decode(char *instruccionDecodificar, int pid) {

    t_instruccion instruccion;

    char **cadena_instruccion = string_split(instruccionDecodificar , " ");

    int tamanio_array = 0;
    while ((cadena_instruccion)[tamanio_array] != NULL) {
        tamanio_array++;
    }

    if(tamanio_array == 3){

        if (strcmp(cadena_instruccion[0], "MOV_IN") == 0) {

            instruccion.tipo_instruccion = MOV_IN;
            instruccion.operando1 = cadena_instruccion[1];
            instruccion.operando2 = cadena_instruccion[2];
            utilizacion_memoria(instruccion, pid);
            //execute(&procesoEjecutando->cpuRegisters, instruccion);

        }

        if (strcmp(cadena_instruccion[0], "MOV_OUT") == 0) {
            
            instruccion.tipo_instruccion = MOV_OUT;
            instruccion.operando1 = cadena_instruccion[1];
            instruccion.operando2 = cadena_instruccion[2];
            utilizacion_memoria(instruccion, pid);
            //execute(&procesoEjecutando->cpuRegisters, instruccion);
            
        }

        if (strcmp(cadena_instruccion[0], "SET") == 0) {
            
            instruccion.tipo_instruccion = SET;
            instruccion.operando1 = cadena_instruccion[1];
            instruccion.operando2 = cadena_instruccion[2];

        }

        if (strcmp(cadena_instruccion[0], "SUM") == 0) {
            
            instruccion.tipo_instruccion = SUM;
            instruccion.operando1 = cadena_instruccion[1];
            instruccion.operando2 = cadena_instruccion[2];

        }

        if (strcmp(cadena_instruccion[0], "SUB") == 0) {
            
            instruccion.tipo_instruccion = SUB;
            instruccion.operando1 = cadena_instruccion[1];
            instruccion.operando2 = cadena_instruccion[2];

            
        }

        if (strcmp(cadena_instruccion[0], "JNZ") == 0) {
            
            instruccion.tipo_instruccion = JNZ;
            instruccion.operando1 = cadena_instruccion[1];
            instruccion.operando2 = cadena_instruccion[2];

        }
        if (strcmp(cadena_instruccion[0], "IO_GEN_SLEEP") == 0) {
            
            instruccion.tipo_instruccion = IO_GEN_SLEEP;
            instruccion.operando1 = cadena_instruccion[1];
            instruccion.operandoNumero = atoi(cadena_instruccion[2]);


        }
       
    }

    if(tamanio_array == 2){

        if (strcmp(cadena_instruccion[0], "RESIZE") == 0) {
            
            instruccion.tipo_instruccion = RESIZE;
            instruccion.operando1 = cadena_instruccion[1];

            //RESIZE (Tamaño): Solicitará a la Memoria ajustar el tamaño del proceso al tamaño pasado por parámetro. 
            //En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto de ejecución al Kernel informando de esta situación.
            //direccion_fisica *direccion_fisica = malloc(sizeof(direccion_fisica));
            
            //enviar a kernel
        }

        if (strcmp(cadena_instruccion[0], "COPY_STRING") == 0) {
            
            instruccion.tipo_instruccion = COPY_STRING;
            instruccion.operando1 = cadena_instruccion[1];
            
        }

        if (strcmp(cadena_instruccion[0], "WAIT") == 0) {
            
            instruccion.tipo_instruccion = WAIT;
            instruccion.operando1 = cadena_instruccion[1];
            
        }

        if (strcmp(cadena_instruccion[0], "SIGNAL") == 0) {
            
            instruccion.tipo_instruccion = SIGNAL;
            instruccion.operando1 = cadena_instruccion[1];
            
        }
    }

    return instruccion;
    
}

void mandarPaqueteaKernel(op_code codigoDeOperacion){
    t_paquete *paquete_Kernel = crear_paquete(codigoDeOperacion);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->PID);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.PC);
    agregar_entero_a_paquete8(paquete_Kernel, procesoEjecutando->cpuRegisters.AX);
    agregar_entero_a_paquete8(paquete_Kernel, procesoEjecutando->cpuRegisters.BX);
    agregar_entero_a_paquete8(paquete_Kernel, procesoEjecutando->cpuRegisters.CX);
    agregar_entero_a_paquete8(paquete_Kernel, procesoEjecutando->cpuRegisters.DX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.EAX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.EBX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.ECX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.EDX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.SI);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.DI);

    enviar_paquete(paquete_Kernel, socketCliente);
    eliminar_paquete(paquete_Kernel);
}
void mandarPaqueteaKernelGenerica(op_code codigoDeOperacion, char* nombreInterfaz, int tiempo){
    t_paquete *paquete_Kernel = crear_paquete(codigoDeOperacion);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->PID);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.PC);
    agregar_entero_a_paquete8(paquete_Kernel, procesoEjecutando->cpuRegisters.AX);
    agregar_entero_a_paquete8(paquete_Kernel, procesoEjecutando->cpuRegisters.BX);
    agregar_entero_a_paquete8(paquete_Kernel, procesoEjecutando->cpuRegisters.CX);
    agregar_entero_a_paquete8(paquete_Kernel, procesoEjecutando->cpuRegisters.DX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.EAX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.EBX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.ECX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.EDX);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.SI);
    agregar_entero_a_paquete32(paquete_Kernel, procesoEjecutando->cpuRegisters.DI);

    //ESPECIFICO PARA GENERICA

    agregar_entero_a_paquete32(paquete_Kernel, (strlen(nombreInterfaz)+1));
    agregar_string_a_paquete(paquete_Kernel, nombreInterfaz);
    agregar_entero_a_paquete32(paquete_Kernel, tiempo);

    enviar_paquete(paquete_Kernel, socketCliente);
    eliminar_paquete(paquete_Kernel);
}

int execute2(t_instruccion instruccion_a_ejecutar,int pid){
    int bloqueado = 0;
    switch(instruccion_a_ejecutar.tipo_instruccion){
        case SET:
        {   
            int valor = atoi(instruccion_a_ejecutar.operando2);
            ejecutar_set(&procesoEjecutando->cpuRegisters, instruccion_a_ejecutar.operando1, (uint8_t)valor);
            break;
        }
        case SUM:
        {
            ejecutar_sum(&procesoEjecutando->cpuRegisters, instruccion_a_ejecutar.operando1, instruccion_a_ejecutar.operando2);
            break;        
        }
        case SUB:
        {
            ejecutar_sub(&procesoEjecutando->cpuRegisters, instruccion_a_ejecutar.operando1, instruccion_a_ejecutar.operando2);
            break;
        }
        case JNZ:
        {
            int valor = atoi(instruccion_a_ejecutar.operando2);
            ejecutar_jnz(&procesoEjecutando->cpuRegisters, instruccion_a_ejecutar.operando1, instruccion_a_ejecutar.operando2);
            break;
        }
        case WAIT:
        {
            char recurso[20];
            sscanf(instruccion_a_ejecutar.operando1, "WAIT %s", recurso);
            ejecutar_wait(&procesoEjecutando->cpuRegisters, recurso);
            break;
        }
        case IO_GEN_SLEEP:
        {
            mandarPaqueteaKernelGenerica(IO_GEN_SLEEP, instruccion_a_ejecutar.operando1, instruccion_a_ejecutar.operandoNumero);
            bloqueado = 1;
            break;
        }
        case RESIZE:
        {
            int tam_nuevo = atoi(instruccion_a_ejecutar.operando1);

            paquete_memoria_resize(pid,tam_nuevo);
            recibir_confirmacion_memoria_resize();
            printf("Instrucción resize realizada!! \n");
            break;
        }
        default:
        {   
            log_error(loggerCpu, "Error");
            break;
        }

    }
    return bloqueado;     
}

void recv_instruccion(int memoria_fd){

	t_list *paquete = recibir_paquete(memoria_fd);

	// Obtener el path del paquete
	char *instruccion_cadena = (char *)list_get(paquete, 0);

	log_info(loggerCpu, "Recibi la instruccion: %s", instruccion_cadena ); 

	char **palabras = string_split(instruccion_cadena , " ");
    
    //int tamanio_array = (sizeof(palabras) / sizeof(palabras[0]));

    int tamanio_array = 0;
    while ((palabras)[tamanio_array] != NULL) {
        tamanio_array++;
    }

	//instruccionActual.tipo_instruccion = malloc(sizeof(char) * (strlen(palabras[1]) + 1));
    //strcpy(instruccionActual.tipo_instruccion, palabras[0]); 

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