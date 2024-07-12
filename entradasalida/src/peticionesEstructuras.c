#include <peticionesEstructuras.h>
#include <string.h>
int obtenerTipoConString(char* tipo){
    if(!strcmp(tipo,"T_GENERICA")){return 0;}
    else if(!strcmp(tipo,"T_STDIN")){return 1;}
    else if(!strcmp(tipo,"T_STDOUT")){return 2;}
    else if(!strcmp(tipo,"T_DFS")){return 3;}
    else {return -1;}
}