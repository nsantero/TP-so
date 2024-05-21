#include "../include/config.h"
#include "../include/instrucciones.h"

extern cpu_config* config;

bool sigo_ejecutando = true;

pcb* recibir_cpu_kernel(int socket){
    t_buffer* bufferEnviar =  buffer_create();
	        
    pcb* pcb1 = malloc(sizeof(pcb));
    int offset = 0;
	
	/* lógica de recepción de pcb */

    return pcb1;
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

	while (sigo_ejecutando /* && chequear_si_hay_interrupcion()*/) {
		t_instruccion *instruccion = fetch(pcb);

		decode(instruccion, pcb);

		free(instruccion);

	}
}

t_instruccion* fetch(pcb *pcb) {
	
	/* recibir instruccion de memoria */

	t_instruccion *instruccion_recibida = convertir_data_a_instruccion();

	pcb->programCounter += 1;

	return instruccion_recibida;
}

t_instruccion* convertir_data_a_instruccion(memoria_cpu_data *data_cpu) {
	t_instruccion *instruccion = malloc(sizeof(*instruccion));

	instruccion->identificador = data_cpu->programCounter;
	instruccion->param1 = data_cpu->param1;
	instruccion->param2 = data_cpu->param2;
	instruccion->param3 = data_cpu->param3;

	return instruccion;
}

void decode(t_instruccion *instruccion, pcb *pcb) {
	int dir_logica;
	int32_t direccionFisica;
	switch (instruccion->identificador) {
		case SET:
			log_info(logger, "PID: %d - Ejecutando: SET - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SET();

		case SUM:
			log_info(logger, "PID: %d - Ejecutando: SUM - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SUM();
			break;

		case SUB:
			log_info(logger, "PID: %d - Ejecutando: SUB - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SUB();
			break;

		case JNZ:
			log_info(logger, "PID: %d - Ejecutando: JNZ - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_JNZ();
			break;

		default:
			break;
	}
}
