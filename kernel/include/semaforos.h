#include <semaphore.h>

extern sem_t *sem_grado_multiprogramacion;
extern sem_t *sem_corto_plazo, *sem_largo_plazo;
extern sem_t *mutex_detener_planificador;
extern sem_t *sem_procesos_new;
extern sem_t *sem_procesos_ready;
extern sem_t *sem_procesos_running;
extern sem_t *sem_proceso_ejecutando;