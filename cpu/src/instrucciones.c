#include "config.h"
#include "instrucciones.h"

extern cpu_config* config;

bool sigo_ejecutando = true;

extern logger;

pcb* recibir_cpu_kernel(int socket) {
    t_buffer* bufferEnviar =  recibir_buffer(socket, bufferEnviar);;
   
    t_identificador id_recibido = stream_recv_de_header(socket);
    
	// Si no llega el BUFFER, tira error
    if (bufferEnviar == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        return NULL;
    }

    pcb* pcb1 = malloc(sizeof(pcb));
    if (!pcb1) {
        log_error(logger, "No se pudo asignar memoria para el PCB");
        free(bufferEnviar);
        return NULL;
    }

    pcb1->registros_cpu = malloc(sizeof(registros_cpu));
    if (!pcb1->registros_cpu) {
        log_error(logger, "No se pudo asignar memoria para registros CPU");
        free(pcb1);
        free(bufferEnviar);
        return NULL;
    }
}	

void atender_dispatch(logger){
	log_info(logger, "CPU escuchando puerto dispatch");

	while (1) {
		pcb* pcb = recibir_cpu_kernel(config->puerto_escucha_dispatch);
		
		ejecutar_ciclo_de_instruccion(pcb,config->puerto_escucha_dispatch );

		liberarPcb(pcb);
	}
}

void ejecutar_ciclo_de_instruccion(pcb *pcb, int socket_kernel) {

	sigo_ejecutando = 1;

	while (sigo_ejecutando /* && chequear_si_hay_interrupcion() para el prox CHECKPOINT*/) {
		t_instruccion *instruccion = fetch(pcb);

		decode(instruccion, pcb);

		free(instruccion);
	}
}

t_instruccion* fetch(pcb *pcb) {
	
	/* recibir instruccion de memoria? */

	t_instruccion *instruccion_recibida = convertir_data_a_instruccion();

	pcb->programCounter += 1;

	return instruccion_recibida;
}

void decode(t_instruccion *instruccion, pcb *pcb) {
	switch (instruccion->identificador) {
		case SET:
			log_info(logger, "PID: %d - Ejecutando: SET - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SET(instruccion->param1, instruccion->param2);

		case SUM:
			log_info(logger, "PID: %d - Ejecutando: SUM - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SUM(instruccion->param1, instruccion->param2);
			break;

		case SUB:
			log_info(logger, "PID: %d - Ejecutando: SUB - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SUB(instruccion->param1, instruccion->param2);
			break;

		case JNZ:
			log_info(logger, "PID: %d - Ejecutando: JNZ - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_JNZ(instruccion->param1, instruccion->param2, pcb->programCounter);
			break;
		
		case IO_GEN_SLEEP:
			log_info(logger, "PID: %d - Ejecutando: IO_GEN_SLEEP - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			int tiempo = 1; //HARDCODEADO, PENSAR LÓGICA
			char* interfaz = "algo"; // PENSAR CÓMO SE PASA LA INTERFAZ
			execute_IO_GEEN_SLEEP(interfaz, tiempo);
			break;

		default:
			break;
	}
}

void execute_SET(uint32_t *registro, int valor){
	*registro = valor;
}

void execute_SUM(uint32_t * registroDestino,uint32_t *registroOrigen){
	*registroDestino += *registroOrigen;
}

void execute_SUB(uint32_t * registroDestino,uint32_t * registroOrigen){
	*registroDestino -= *registroOrigen;
}

void execute_JNZ(uint32_t * registro,uint32_t valor, uint32_t pc){
	if(*registro!=0)
		pc = valor;
}

void execute_IO_GEEN_SLEEP(interfaz, tiempo){
	
}