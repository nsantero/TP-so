#include "configs.h"

t_config* configKernel = NULL;
t_log* loggerKernel = NULL;
t_configKernel configuracionKernel;

void armarConfig(){
    configKernel = config_create("../kernel/kernel.config");
    configuracionKernel.PUERTO_ESCUCHA = config_get_string_value(configKernel, "PUERTO_ESCUCHA");
    configuracionKernel.IP_MEMORIA = config_get_string_value(configKernel, "IP_MEMORIA");
    configuracionKernel.PUERTO_MEMORIA = config_get_string_value(configKernel, "PUERTO_MEMORIA");
    configuracionKernel.IP_CPU = config_get_string_value(configKernel, "IP_CPU");
    configuracionKernel.PUERTO_CPU_DISPATCH = config_get_string_value(configKernel, "PUERTO_CPU_DISPATCH");
    configuracionKernel.PUERTO_CPU_INTERRUPT = config_get_string_value(configKernel, "PUERTO_CPU_INTERRUPT");
    configuracionKernel.ALGORITMO_PLANIFICACION = config_get_string_value(configKernel, "ALGORITMO_PLANIFICACION");
    configuracionKernel.QUANTUM = config_get_int_value(configKernel, "QUANTUM");
    configuracionKernel.GRADO_MULTIPROGRAMACION = config_get_int_value(configKernel, "GRADO_MULTIPROGRAMACION");
    configuracionKernel.RECURSOS = config_get_array_value(configKernel, "RECURSOS");
    configuracionKernel.INSTANCIAS_RECURSOS = config_get_array_value(configKernel, "INSTANCIAS_RECURSOS");

    inicializarRecursos(&configuracionKernel, configKernel);

    log_info(loggerKernel, "Configuracion guardada");
}

void iniciarLogger(){
    loggerKernel = log_create("../kernel/kernel.log","Kernel", 0, LOG_LEVEL_INFO);
    log_info(loggerKernel,"Logger creado");
}

void inicializarRecursos(t_configKernel* config, t_config* configFile) {
    config->RECURSOS = list_create();
    char** recursos_config = config_get_array_value(configFile, "RECURSOS");
    char** instancias_recursos_config = config_get_array_value(configFile, "INSTANCIAS_RECURSOS");

    for (int i = 0; recursos_config[i] != NULL && instancias_recursos_config[i] != NULL; i++) {
        t_recurso* recurso = malloc(sizeof(t_recurso));
        recurso->nombre_recurso = strdup(recursos_config[i]);
        recurso->cantidad_instancias = atoi(instancias_recursos_config[i]);
        list_add(config->RECURSOS, recurso);
    }

    // Libera la memoria utilizada por los arrays de recursos e instancias
    int i = 0;
    while (recursos_config[i] != NULL) {
        free(recursos_config[i]);
        free(instancias_recursos_config[i]);
        i++;
    }
    free(recursos_config);
    free(instancias_recursos_config);
}


void liberarRecursos(t_configKernel* config) {
    for (int i = 0; i < list_size(config->RECURSOS); i++) {
        t_recurso* recurso = list_get(config->RECURSOS, i);
        free(recurso->nombre_recurso);
        free(recurso);
    }
    list_destroy(config->RECURSOS);
}
void imprimirConfig(t_configKernel* config) {
    printf("PUERTO_ESCUCHA: %s\n", config->PUERTO_ESCUCHA);
    printf("IP_MEMORIA: %s\n", config->IP_MEMORIA);
    printf("PUERTO_MEMORIA: %s\n", config->PUERTO_MEMORIA);
    printf("IP_CPU: %s\n", config->IP_CPU);
    printf("PUERTO_CPU_DISPATCH: %s\n", config->PUERTO_CPU_DISPATCH);
    printf("PUERTO_CPU_INTERRUPT: %s\n", config->PUERTO_CPU_INTERRUPT);
    printf("ALGORITMO_PLANIFICACION: %s\n", config->ALGORITMO_PLANIFICACION);
    printf("QUANTUM: %d\n", config->QUANTUM);
    printf("GRADO_MULTIPROGRAMACION: %d\n", config->GRADO_MULTIPROGRAMACION);

    printf("RECURSOS:\n");
    for (int i = 0; i < list_size(config->RECURSOS); i++) {
        t_recurso* recurso = list_get(config->RECURSOS, i);
        printf("  Nombre: %s, Instancias: %d\n", recurso->nombre_recurso, recurso->cantidad_instancias);
    }
}
void limpiarConfig() {
    liberarRecursos(&configuracionKernel);
    config_destroy(configKernel);
}