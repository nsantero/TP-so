#include <loggersIO.h>

t_log *loggerIO;
char* path;

void inicializarLogger(){
    path=string_from_format("%s%s",pathADirectorio,"loggerDeIO.log");
    loggerIO=log_create(path,"EntradaYSalida",false,2);
}

void inicializarLoggerDeInterfaz(char* nombreDeInterfaz){
    path=string_from_format("%s%s%s",pathADirectorio,nombreDeInterfaz,".log");
    loggerIO=log_create(path,"EntradaYSalida",false,2);
}

void cerrarLogger(){
    log_destroy(loggerIO);
}