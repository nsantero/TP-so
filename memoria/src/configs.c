#include <configs.h>

t_config* configMemoria = NULL;
t_log* loggerMemoria = NULL;
memoria_config configuracionMemoria;

void armarConfigMemoria(){
    configMemoria = config_create("../memoria/memoria.config");
	configuracionMemoria.PUERTO_ESCUCHA = config_get_string_value(configMemoria, "PUERTO_ESCUCHA");
    configuracionMemoria.TAM_MEMORIA = config_get_int_value(configMemoria,"TAM_MEMORIA");
    configuracionMemoria.TAM_PAGINA = config_get_int_value(configMemoria,"TAM_PAGINA");
    configuracionMemoria.PATH_INSTRUCCIONES = config_get_string_value(configMemoria,"PATH_INSTRUCCIONES");
    configuracionMemoria.RETARDO_RESPUESTA = config_get_int_value(configMemoria,"RETARDO_RESPUESTA");
	
    log_info(loggerMemoria, "Configuracion guardada");
}

void iniciarLoggerMemoria(){
    loggerMemoria = log_create("../memoria/memoria.log","Memoria", 0, LOG_LEVEL_INFO);
    log_info(loggerMemoria,"Logger Memoria creado");
}