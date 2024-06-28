#include <commons/config.h>
#include <commons/log.h>

typedef struct
{		
    char* PUERTO_ESCUCHA; 
    int TAM_MEMORIA;
    int TAM_PAGINA; 
    char* PATH_INSTRUCCIONES;
    int RETARDO_RESPUESTA; 		
   
} memoria_config;

extern memoria_config configuracionMemoria;
extern t_config* configMemoria;

t_log* loggerMemoria;

void armarConfigMemoria();
void iniciarLoggerMemoria();