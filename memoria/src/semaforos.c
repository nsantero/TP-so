#include <semaforos.h>
pthread_mutex_t mutexSocketKernel = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexSocketCpu = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t accesoAMemoria = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t listaProcesosActivos = PTHREAD_MUTEX_INITIALIZER;

