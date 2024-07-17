#include <peticionesEstructuras.h>
#include <string.h>
int obtenerTipoConString(char* tipo){
    if(!strcmp(tipo,"GENERICA")){return 0;}
    else if(!strcmp(tipo,"STDIN")){return 1;}
    else if(!strcmp(tipo,"STDOUT")){return 2;}
    else if(!strcmp(tipo,"DIALFS")){return 3;}
    else {return -1;}
}