#include <sigInt.h>

void handleSiginitIO(){


    //cerrar hilo?
    //free de todo lo q tenga q hacer free TODO
    //posiblemente con un switch del tipo de interfaz q este manejando

    
    log_info(loggerIO,"Se desconecta la interfaz.");
    cerrarLogger();
    exit(0);
}