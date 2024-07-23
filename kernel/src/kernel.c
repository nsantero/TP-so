#include <utils.h>
#include <semaforos.h>
#include <kernel.h>
#include <planificadores.h>
#include <conexiones.h>
#include <configs.h>

// LISTA DE ESTADOS

t_list* lista_NEW;
t_list* lista_READY;
t_list* lista_READYPRI;
t_list* lista_BLOCKED;
t_list* lista_EXIT;
t_list* lista_BLOCKED_RECURSOS;
t_list* lista_RUNNING;
t_list* interfacesConectadas;

// inicializar grado de multiprogramacion 

int grado_multiprogramacion = 0;

void inicializarListas() {
    lista_NEW = list_create();
    lista_READY = list_create();
    lista_READYPRI = list_create();
    lista_EXIT = list_create();
    lista_BLOCKED = list_create();
    lista_BLOCKED_RECURSOS = list_create();
    lista_RUNNING = list_create();
    interfacesConectadas = list_create();
}

//Generar PID

int pid_counter = 1;
int quantum = 2;
char* algoritmo_planificacion = NULL;

// PCB 
    char *pid = NULL;


int pidActual = 0;

int generarPID() {
    pidActual += 1;
    return pidActual;
}

PCB* crearPCB(char* path) {
    printf("Creando PCB... \n");
    PCB* nuevoPCB = malloc(sizeof(PCB)); //reserva de memoria
     if (nuevoPCB == NULL) {
        return NULL; 
    }
    nuevoPCB -> PID = generarPID();
    nuevoPCB -> recursosEnUso = list_create();
    nuevoPCB -> recursoBloqueante = NULL;
    nuevoPCB -> cpuRegisters.PC = 0;
    nuevoPCB -> cpuRegisters.AX = 0;
    nuevoPCB -> cpuRegisters.BX = 0;
    nuevoPCB -> cpuRegisters.CX = 0;
    nuevoPCB -> cpuRegisters.DX = 0;
    nuevoPCB -> cpuRegisters.EAX = 0;
    nuevoPCB -> cpuRegisters.EBX = 0;
    nuevoPCB -> cpuRegisters.ECX = 0;
    nuevoPCB -> cpuRegisters.EDX = 0;
    nuevoPCB -> cpuRegisters.SI = 0;
    nuevoPCB -> cpuRegisters.DI = 0;
    nuevoPCB -> quantum = configuracionKernel.QUANTUM;
    nuevoPCB -> estado = NEW;
    pthread_mutex_lock(&mutexListaNew);
    list_add(lista_NEW, nuevoPCB);
    sem_post(&semListaNew);
    pthread_mutex_unlock(&mutexListaNew);

    paquete_memoria_crear_proceso(nuevoPCB->PID, path);
    
    log_info(loggerKernel, "Se creó el PCB del nuevo proceso, PID %d", nuevoPCB -> PID);
    return nuevoPCB;
}
void eliminarProceso(PCB* proceso){
    free(proceso);
}
void actualizarProceso(PCB* procesoCPU, PCB* procesoKernel){
    procesoKernel->cpuRegisters.PC = procesoCPU->cpuRegisters.PC;
    procesoKernel->cpuRegisters.AX = procesoCPU->cpuRegisters.AX;
    procesoKernel->cpuRegisters.BX = procesoCPU->cpuRegisters.BX;
    procesoKernel->cpuRegisters.CX = procesoCPU->cpuRegisters.CX;
    procesoKernel->cpuRegisters.DX = procesoCPU->cpuRegisters.DX;
    procesoKernel->cpuRegisters.EAX = procesoCPU->cpuRegisters.EAX;
    procesoKernel->cpuRegisters.EBX = procesoCPU->cpuRegisters.EBX;
    procesoKernel->cpuRegisters.ECX = procesoCPU->cpuRegisters.ECX;
    procesoKernel->cpuRegisters.EDX = procesoCPU->cpuRegisters.EDX;
    procesoKernel->cpuRegisters.SI = procesoCPU->cpuRegisters.SI;
    procesoKernel->cpuRegisters.DI = procesoCPU->cpuRegisters.DI;
}
int leer_grado_multiprogramación() {
    return grado_multiprogramacion = configuracionKernel.GRADO_MULTIPROGRAMACION;
}

void finalizarProceso(uint32_t pid){
    PCB * proceso = NULL;
    pthread_mutex_lock(&mutexListaNew);
    pthread_mutex_lock(&mutexListaReady);
    pthread_mutex_lock(&mutexListaReadyPri);
    pthread_mutex_lock(&mutexListaBlocked);
    pthread_mutex_lock(&mutexListaBlockedRecursos);
    pthread_mutex_lock(&mutexListaRunning);
    pthread_mutex_lock(&mutexListaExit);

    proceso=buscarProcesoPID(pid, lista_NEW);
    if(proceso != NULL){
        proceso->estado = EXIT;
        list_add(lista_EXIT, proceso);
        paquete_memoria_finalizar_proceso(pid);
    }
    
    proceso=buscarProcesoPID(pid, lista_READY);
    if(proceso != NULL){
        proceso->estado = EXIT;
        list_add(lista_EXIT, proceso);
        paquete_memoria_finalizar_proceso(pid);
    }
    proceso=buscarProcesoPID(pid, lista_READYPRI);
    if(proceso != NULL){
        proceso->estado = EXIT;
        list_add(lista_EXIT, proceso);
        paquete_memoria_finalizar_proceso(pid);
    }
    
    proceso=buscarProcesoPID(pid, lista_BLOCKED);
    if(proceso != NULL){
        proceso->estado = EXIT;
        list_add(lista_EXIT, proceso);
        paquete_memoria_finalizar_proceso(pid);
    }

    proceso=buscarProcesoPID(pid, lista_BLOCKED_RECURSOS);
    if(proceso != NULL){
        proceso->estado = EXIT;
        list_add(lista_EXIT, proceso);
        //Recurso* recurso=buscarRecurso(proceso->recursoBloqueante);
        //recurso->instancias++;
        for(int i=0; i<list_size(proceso->recursosEnUso); i++){
            Recurso *recursoALiberar = list_get(proceso->recursosEnUso,i);
            for(int j=0; j<list_size(lista_BLOCKED_RECURSOS); j++){
                PCB *procesoBloqueado = list_get(lista_BLOCKED_RECURSOS, j);
                if (strcmp(recursoALiberar->nombre, procesoBloqueado->recursoBloqueante) == 0){
                    procesoBloqueado = list_remove(lista_BLOCKED_RECURSOS, j);
                    procesoBloqueado->estado = READY;
                    list_add(lista_READY, procesoBloqueado);
                    sem_post(&semListaReady);
                    log_info(loggerKernel,"Proceso %d desbloqueado por finalizacion de proceso: %d de recurso %s\n", procesoBloqueado->PID, proceso->PID, procesoBloqueado->recursoBloqueante);
                }
            }
            
        }
        
        paquete_memoria_finalizar_proceso(pid);
    }
    
    proceso=buscarProcesoPIDSinRemover(pid, lista_RUNNING);
    if(proceso != NULL){
        paquete_CPU_interrumpir_proceso_finalizado(pid);
    }
    
    pthread_mutex_unlock(&mutexListaNew);
    pthread_mutex_unlock(&mutexListaReady);
    pthread_mutex_unlock(&mutexListaReadyPri);
    pthread_mutex_unlock(&mutexListaBlocked);
    pthread_mutex_unlock(&mutexListaBlockedRecursos);
    pthread_mutex_unlock(&mutexListaRunning);
    pthread_mutex_unlock(&mutexListaExit);

    return;
}
void paquete_CPU_interrumpir_proceso_finalizado(int pid){
    t_paquete *paquete_CPU = crear_paquete(INTERRUMPIR_PROCESO);

    agregar_entero_a_paquete32(paquete_CPU, pid);
    enviar_paquete(paquete_CPU, cpu_interrupt_fd);
    eliminar_paquete(paquete_CPU);
}

PCB* buscarProcesoPID(uint32_t pid, t_list* lista){
	PCB *proceso;
	if(list_size(lista) != 0){
		for (int i = 0; i < list_size(lista); i++)
		{
			proceso=list_get(lista, i);
			if(proceso->PID == pid){
				proceso = list_remove(lista, i);
				return proceso;
			}
		}
	}
	return NULL;
}
PCB* buscarProcesoPIDSinRemover(uint32_t pid, t_list* lista){
	PCB *proceso;
	if(list_size(lista) != 0){
		for (int i = 0; i < list_size(lista); i++)
		{
			proceso=list_get(lista, i);
			if(proceso->PID == pid){
				proceso = list_get(lista, i);
				return proceso;
			}
		}
	}
	return NULL;
}


// Definiciones de las funciones crear_paquete, agregar_a_paquete, enviar_paquete, eliminar_paquete
// Definiciones de los IDs de paquetes CREAR_PROCESO y DATOS_DEL_PROCESO

void paquete_memoria_crear_proceso(int PID_paquete, char* path_paquete){

    t_paquete *paquete_memoria = crear_paquete(CREAR_PROCESO);
    
    // Agregar el path al paquete
    agregar_entero_a_paquete32(paquete_memoria, PID_paquete);
    agregar_entero_a_paquete32(paquete_memoria, (strlen(path_paquete)+1));
    agregar_string_a_paquete(paquete_memoria, path_paquete);
    
    // Pasar PID y txt a memoria
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);

}

void paquete_memoria_finalizar_proceso(int PID_paquete){

    t_paquete *paquete_memoria = crear_paquete(FINALIZAR_PROCESO);

    // Agregar el path al paquete
    agregar_entero_a_paquete32(paquete_memoria, PID_paquete);

    // Pasar PID y txt a memoria
    enviar_paquete(paquete_memoria, memoria_fd);
    eliminar_paquete(paquete_memoria);

}
