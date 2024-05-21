#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_ 

#include "config.h"
#include "cpu.h"

typedef struct{
	//campos de instruccion
	int pid;
	int programCounter;
	char* param1;
	char* param2;
	char* param3;
	int direccion;
	uint32_t registroValor;

} memoria_cpu_data;

typedef enum{
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

#endif /* INSTRUCCIONES_H_ */