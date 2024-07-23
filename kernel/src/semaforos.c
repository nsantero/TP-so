#include <semaforos.h>
#include <conexiones.h>

pthread_mutex_t mutexListaNew = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaReady = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaReadyPri = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaBlocked = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaBlockedRecursos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaRunning= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexListaExit = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutexHiloQuantum = PTHREAD_MUTEX_INITIALIZER;

sem_t semListaNew;
sem_t semListaReady;
sem_t semListaRunning;

sem_t semPlaniNew;
sem_t semPlaniReady;
sem_t semPlaniBlocked;
sem_t semPlaniRunning;
sem_t semPlaniReadyClock;
sem_t semPlaniBlockedRecursos;

sem_t semIOGEN;

// VER 
sem_t sem_grado_multiprogramacion;
//sem_t sem_corto_plazo, *sem_largo_plazo;
//sem_t mutex_detener_planificador;


void inicializar_sem_planificadores()
{
    sem_init(&semListaNew, 0, 0);
    sem_init(&semListaReady, 0, 0);
    sem_init(&semListaRunning, 0, 1);

    sem_init(&semPlaniNew,0,1);
    sem_init(&semPlaniReady,0,1);
    sem_init(&semPlaniBlocked,0,1);
    sem_init(&semPlaniRunning,0,1);
    sem_init(&semPlaniReadyClock,0,1);
    sem_init(&semPlaniBlockedRecursos,0,1);

    sem_init(&semIOGEN,0,1);
}
