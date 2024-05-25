#include "instrucciones.h"

///////////////////////////////////////////////////////////// CICLO DE INSTRUCCIÓN ///////////////////////////////////////////////////////////////////

void ciclo_instruccion(pcb* contexto, int cliente_socket_dispatch, t_log* logger) {
    //log_info(logger,ANSI_COLOR_BLUE "Inicio del ciclo de instrucción");
    while (cliente_socket_dispatch != -1) {

    t_instruccion * instruccion = malloc(sizeof(t_instruccion));

    if(instruccion){
		decode(instruccion,contexto);
        contexto->programCounter++;
           
        /*free(instruccion->opcode);
        free(instruccion->operando1);
        free(instruccion->operando2);
        free(instruccion);*/
    }
}

/*t_instruccion* fetch(pcb *pcb){
	
	 recibir instruccion de memoria? 
}; */


void decode(t_instruccion *instruccion, pcb *pcb) {
	switch (instruccion->identificador) {
		case SET:
			log_info(cpu_log, "PID: %d - Ejecutando: SET - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SET(instruccion->param1, instruccion->param2);

		case SUM:
			log_info(cpu_log, "PID: %d - Ejecutando: SUM - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SUM(instruccion->param1, instruccion->param2);
			break;

		case SUB:
			log_info(cpu_log, "PID: %d - Ejecutando: SUB - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_SUB(instruccion->param1, instruccion->param2);
			break;

		case JNZ:
			log_info(cpu_log, "PID: %d - Ejecutando: JNZ - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			execute_JNZ(instruccion->param1, instruccion->param2, pcb->programCounter);
			break;
		
		case IO_GEN_SLEEP:
			log_info(cpu_log, "PID: %d - Ejecutando: IO_GEN_SLEEP - %s - %s", pcb->pid, instruccion->param1, instruccion->param2);
			int tiempo = 1; //HARDCODEADO, PENSAR LÓGICA
			char* interfaz = "algo"; // PENSAR CÓMO SE PASA LA INTERFAZ
			execute_IO_GEEN_SLEEP(interfaz, tiempo);

		default:
			break;
	}
}

//////////////////////////////////////////////////////////////// INSTRUCCIONES //////////////////////////////////////////////////////////////////////

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