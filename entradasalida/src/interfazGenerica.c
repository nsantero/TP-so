#include <interfazGenerica.h>
#include <unistd.h>

t_list * cola_procesos_ig = NULL;

pthread_mutex_t mutex_cola_ig = PTHREAD_MUTEX_INITIALIZER;

sem_t sem_hay_en_cola ;


void inicializar_sem_cola_ig()
{
	sem_init(&sem_hay_en_cola, 0, 0);
}



Interfaz generarNuevaInterfazGenerica(char* nombre,char* pathConfiguracion){

    t_config * configuracion;
    configuracion = config_create(pathConfiguracion);

    Interfaz aDevolver;
    


    aDevolver.nombre=nombre;
    aDevolver.tipoInterfaz=config_get_string_value(configuracion,"TIPO_INTERFAZ");
    aDevolver.tiempoUnidadTrabajo=config_get_int_value(configuracion,"TIEMPO_UNIDAD_TRABAJO");
    aDevolver.ipKernel=config_get_string_value(configuracion,"IP_KERNEL");
    aDevolver.puertoKernel=config_get_string_value(configuracion,"PUERTO_KERNEL");
    aDevolver.blockCount=NULL;
    aDevolver.blockSize=NULL;
    aDevolver.ipMemoria=NULL;
    aDevolver.puertoMemoria=NULL;
    aDevolver.pathBaseDialfs=NULL;
    aDevolver.retrasoCompactacion=NULL;
    

    return aDevolver;
}

void manejarPeticionInterfazGenerica(unsigned unidadesAEsperar,Interfaz interfaz){

    usleep(unidadesAEsperar*interfaz.tiempoUnidadTrabajo*1000);

}
