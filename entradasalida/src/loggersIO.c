#include <loggersIO.h>

t_log *loggerIO;
char* path;

void inicializarLogger(){
    loggerIO=log_create(path,"EntradaYSalida",true,2);
}