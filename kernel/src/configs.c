#include "configs.h"

t_config* configKernel = NULL;
t_log* loggerKernel = NULL;
t_configKernel configuracionKernel;

void armarConfig(){
    configKernel = config_create("../kernel/kernel.config");
    configuracionKernel.PUERTO_ESCUCHA = config_get_string_value(configKernel, "PUERTO_ESCUCHA");
    configuracionKernel.IP_MEMORIA = config_get_string_value(configKernel, "IP_MEMORIA");
    configuracionKernel.PUERTO_MEMORIA = config_get_string_value(configKernel, "PUERTO_MEMORIA");
    configuracionKernel.IP_CPU = config_get_string_value(configKernel, "IP_CPU ");
    configuracionKernel.PUERTO_CPU_DISPATCH = config_get_string_value(configKernel, "PUERTO_CPU_DISPATCH");
    configuracionKernel.PUERTO_CPU_INTERRUPT = config_get_string_value(configKernel, "PUERTO_CPU_INTERRUPT");
    configuracionKernel.ALGORITMO_PLANIFICACION = config_get_string_value(configKernel, "ALGORITMO_PLANIFICACION");
    configuracionKernel.QUANTUM = config_get_int_value(configKernel, "QUANTUM");
    configuracionKernel.GRADO_MULTIPROGRAMACION = config_get_int_value(configKernel, "GRADO_MULTIPROGRAMACION");

    log_info(loggerKernel, "Configuracion guardada");
}

void iniciarLogger(){
    loggerKernel = log_create("../kernel/kernel.log","Kernel", 0, LOG_LEVEL_INFO);
    log_info(loggerKernel,"Logger creado");
}