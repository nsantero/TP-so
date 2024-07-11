#include <loggersIO.h>

t_log *loggerIO;
char* path;

void inicializarLogger(){
    loggerIO=log_create(path,"EntradaYSalida",false,2);
}

void cerrarLogger(){
    log_destroy(loggerIO);
}