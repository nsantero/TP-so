#include <loggersIO.h>

t_log *loggerIO;
char* path;

void inicializarLogger(){
    path=string_from_format("%s%s",pathADirectorio,"loggerDeIO");
    loggerIO=log_create(path,"EntradaYSalida",false,2);
}

void cerrarLogger(){
    log_destroy(loggerIO);
}