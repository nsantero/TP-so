#include <semaphore.h>
#include <semaforos.h>

void inicializar_sem_planificadores()
{
	sem_corto_plazo = malloc(sizeof(sem_t));
	sem_init(sem_corto_plazo, 0, 0);

	sem_largo_plazo = malloc(sizeof(sem_t));
	sem_init(sem_largo_plazo, 0, 0);

	sem_grado_multiprogramacion = malloc(sizeof(sem_t));
	sem_init(sem_grado_multiprogramacion, 0, leer_grado_multiprogramación());

	mutex_detener_planificador = malloc(sizeof(sem_t));
	sem_init(mutex_detener_planificador, 0, 1);

    sem_procesos_new = malloc (sizeof(sem_t));
    sem_init(sem_procesos_new, 0, 1);

    sem_procesos_ready = malloc (sizeof(sem_t));
    sem_init(sem_procesos_ready, 0, 1);

    sem_procesos_running = malloc (sizeof(sem_t));
    sem_init(sem_procesos_running, 0, 1);

    sem_proceso_ejecutando = malloc (sizeof(sem_t));
    sem_init(sem_proceso_ejecutando, 0, 0);
}