#include <semaforos.h>
#include <conexiones.h>

pthread_mutex_t mutexListaNew = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaReady = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaBlocked = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaRunning= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaExit = PTHREAD_MUTEX_INITIALIZER;

sem_t semListaNew;
sem_t semListaReady;
sem_t semListaRunning;

// VER 
sem_t sem_grado_multiprogramacion;
//sem_t sem_corto_plazo, *sem_largo_plazo;
//sem_t mutex_detener_planificador;


void inicializar_sem_planificadores()
{
    sem_init(&semListaNew, 0, 0);
    sem_init(&semListaReady, 0, 0);
    sem_init(&semListaRunning, 0, 0);

	/*sem_init(&sem_corto_plazo, 0, 0);
	sem_init(&sem_largo_plazo, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, leer_grado_multiprogramación());
	sem_init(&mutex_detener_planificador, 0, 1);
    sem_init(&sem_procesos_new, 0, 1);
    sem_init(&sem_procesos_ready, 0, 1);
    sem_init(&sem_procesos_running, 0, 1);
    sem_init(&sem_proceso_ejecutando, 0, 0);*/
}
