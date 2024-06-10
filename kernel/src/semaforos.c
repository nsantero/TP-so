#include <semaforos.h>
#include <conexiones.h>

sem_t sem_grado_multiprogramacion;
sem_t sem_corto_plazo, *sem_largo_plazo;
sem_t mutex_detener_planificador;
sem_t sem_procesos_new;
sem_t sem_procesos_ready;
sem_t sem_procesos_running;
sem_t sem_proceso_ejecutando;


void inicializar_sem_planificadores()
{
	sem_init(&sem_corto_plazo, 0, 0);
	sem_init(&sem_largo_plazo, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, leer_grado_multiprogramación());
	sem_init(&mutex_detener_planificador, 0, 1);
    sem_init(&sem_procesos_new, 0, 1);
    sem_init(&sem_procesos_ready, 0, 1);
    sem_init(&sem_procesos_running, 0, 1);
    sem_init(&sem_proceso_ejecutando, 0, 0);
}
