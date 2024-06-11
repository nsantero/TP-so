#include <interfazGenerica.h>
#include <unistd.h>

Interfaz interfaz_generica;
t_list * cola_procesos_ig = NULL;
pthread_mutex_t mutex_cola_ig = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_hay_en_cola ;


void inicializar_sem_cola_ig()
{
	sem_init(&sem_hay_en_cola, 0, 0);
}

void* manejo_interfaz_generica(){

    Peticion_Interfaz_Generica* peticion_ig;

    while(1){
        sem_wait(&sem_hay_en_cola);
        pthread_mutex_lock(&mutex_cola_ig);
        peticion_ig = list_remove(cola_procesos_ig,0);
        pthread_mutex_unlock(&mutex_cola_ig);

        manejarPeticionInterfazGenerica(peticion_ig->unidades_de_trabajo, interfaz_generica);
        //Avisar a kernel que termino 


    }

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
