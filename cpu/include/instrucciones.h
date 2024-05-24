#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_ 
#include "cpu.h"

// ESTRUCTURAS //
typedef enum{
	EXECUTE,
	SET,
	SUM,
	SUB,
	JNZ,
    IO_GEN_SLEEP
} t_identificador;

typedef struct {
	t_identificador identificador;
	char* param1;
	char* param2;
	char* param3;
} t_instruccion;

// VARIABLES //
extern cpu_config* config_cpu;
bool sigo_ejecutando = true;
extern cpu_log;

// FUNCIONES //
pcb* recibir_cpu_kernel(int socket);
void atender_dispatch(logger);
void ejecutar_ciclo_de_instruccion(pcb *pcb, int socket_kernel);
t_instruccion* fetch(pcb *pcb);
void decode(t_instruccion *instruccion, pcb *pcb);
void execute_SET(uint32_t *registro, int valor);
void execute_SUM(uint32_t * registroDestino,uint32_t *registroOrigen);
void execute_SUB(uint32_t * registroDestino,uint32_t * registroOrigen);
void execute_JNZ(uint32_t * registro,uint32_t valor, uint32_t pc);

#endif /* INSTRUCCIONES_H_ */