#include "configs-cpu.h"

t_config* configCpu = NULL;
t_log* loggerCpu = NULL;
t_configCpu configuracionCpu;

void armarConfig(){
    configCpu= config_create("../cpu/cpu.config");
    configuracionCpu.IP_MEMORIA = config_get_string_value(configCpu, "IP_MEMORIA");
    configuracionCpu.PUERTO_MEMORIA = config_get_string_value(configCpu, "PUERTO_MEMORIA");
    configuracionCpu.PUERTO_ESCUCHA_DISPATCH = config_get_string_value(configCpu, "PUERTO_ESCUCHA_DISPATCH");
    configuracionCpu.PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(configCpu, "PUERTO_ESCUCHA_INTERRUPT");
    configuracionCpu.CANTIDAD_ENTRADAS_TLB = config_get_int_value(configCpu, "CANTIDAD_ENTRADAS_TLB");
    configuracionCpu.ALGORITMO_TLB = config_get_string_value(configCpu, "ALGORITMO_TLB");

    log_info(loggerCpu, "Configuracion guardada");
}

void iniciarLogger(){
    loggerCpu = log_create("../cpu/cpu.log","Cpu", 0, LOG_LEVEL_INFO);
    log_info(loggerCpu,"Logger creado");
}