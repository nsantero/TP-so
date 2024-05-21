#include "../../shared/include/shared.h"

typedef struct
{
    char* ip_memoria;
    char* puerto_memoria;
    char* puerto_escucha_dispatch;
    char* puerto_escucha_interrupt;
    int cant_enradas_tlb;
    char* algoritmo_tlb;

} cpu_config;

cpu_config config_valores;

void cargar_configuracion(char* archivo_configuracion);

