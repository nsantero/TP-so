#include <conexiones.h>
#include <configs.h>

int memoria_fd;
int cpu_dispatch_fd=0;
int cpu_interrupt_fd=0;
int server_fd;
char* server_name = "kernel";
int socketCliente;

t_list *interfacesConectadas;

////////////////////////////////////////////////////////// PROCESO CONEXION //////////////////////////////////////////////////////////

void* conexionesDispatch()
{
	
	while (1)
	{

		t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

        recv(cpu_dispatch_fd, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(cpu_dispatch_fd, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(cpu_dispatch_fd, paquete->buffer->stream, paquete->buffer->size, 0);
		void *stream = paquete->buffer->stream;
		PCB *procesoCPU;
		PCB* procesoKernel;
		switch (paquete->codigo_operacion)
		{
			case PROCESO_EXIT:
			{
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				pthread_mutex_lock(&mutexListaRunning);
				pthread_mutex_lock(&mutexListaExit);
				procesoKernel = list_remove(lista_RUNNING, 0);
				if(procesoCPU->PID == procesoKernel->PID){
					actualizarProceso(procesoCPU, procesoKernel);
				}
				else{
					log_error(loggerKernel,"los procesos que se quieren actualizar son distintos el de CPU:%d, Kernel:%d",procesoCPU->PID, procesoKernel->PID);

				}
				procesoKernel->estado = EXIT;
				list_add(lista_EXIT, procesoKernel); 
				//proceso = cambiarAExitDesdeRunning(lista_RUNNING);
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaExit);
				sem_post(&semListaRunning);
				//mutex conexion memoria
				paquete_memoria_finalizar_proceso(procesoKernel->PID);

				log_info(loggerKernel, "Se elimino el proceso con pid: %d\n", procesoKernel->PID);

				//eliminarProceso(procesoKernel);
				break;
			}
			case PROCESO_INTERRUMPIDO_CLOCK:
			{
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				
				pthread_mutex_lock(&mutexListaRunning);
				pthread_mutex_lock(&mutexListaReady);
				procesoKernel = list_remove(lista_RUNNING, 0);
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "RR") || !strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					actualizarProceso(procesoCPU, procesoKernel);
					procesoKernel->estado = READY;
					list_add(lista_READY, procesoKernel);
					sem_post(&semListaReady);
				}
				else{
					log_error(loggerKernel,"Solo se tienen que interrumpir los procesos que usan RR o VRR");
				}
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaReady);
				sem_post(&semListaRunning);

				log_info(loggerKernel, "El proceso con pid:%d se interrumpio por fin de qunatum\n", procesoKernel->PID);
				break;
			}
			case PROCESO_WAIT:
			{
				// Recibir nombre del recurso y PID
                char nombre_recurso[50];
                int pid;
                memcpy(nombre_recurso, stream, 50);
                stream += 50;
                memcpy(&pid, stream, sizeof(int));
                wait_recurso(nombre_recurso, pid);
				break;
			}
			case PROCESO_SIGNAL:
			{
				// Recibir nombre del recurso y PID
                char nombre_recurso[50];
                int pid;
                memcpy(nombre_recurso, stream, 50);
                stream += 50;
                memcpy(&pid, stream, sizeof(int));
                signal_recurso(nombre_recurso, pid);
				break;
			}
			case RESIZE_ERROR:
			{
				break;
			}
			// INSTRUCCIONES I/O
			case IO_GEN_SLEEP:
			{
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "RR") || !strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					terminarHiloQuantum();
				}
				pthread_mutex_lock(&mutexListaRunning);
				pthread_mutex_lock(&mutexListaBlocked);
				procesoKernel = list_remove(lista_RUNNING, 0);
				actualizarProceso(procesoCPU, procesoKernel);
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					temporal_stop(tiempoVRR);
					tiempoEjecutando = temporal_gettime(tiempoVRR);
					temporal_destroy(tiempoVRR);
					procesoKernel->quantum -= tiempoEjecutando;
				}
				procesoKernel->estado = BLOCKED;
				list_add(lista_BLOCKED, procesoKernel);
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaBlocked);
				sem_post(&semListaRunning);
				log_info(loggerKernel, "El proceso con pid:%d se interrumpio por IO_GEN_SLEEP\n", procesoKernel->PID);

				Peticion_Interfaz_Generica interfazGenerica;

				PCB* proceso;

				int pathLength;
				
				
				memcpy(&interfazGenerica.unidades_de_trabajo, stream, sizeof(int));
				stream += sizeof(int);
				memcpy(&interfazGenerica.PID, stream, sizeof(int));
				stream += sizeof(int);
				memcpy(&pathLength, stream, sizeof(int));
				interfazGenerica.nombre_interfaz = malloc(pathLength);
				memcpy(interfazGenerica.nombre_interfaz, stream, pathLength);

				//MUTEX
				if(existeInterfaz(interfazGenerica.nombre_interfaz)){
					t_paquete* paqueteIOGen=crear_paquete(IO_GEN_SLEEP);
					agregar_a_paquete(paqueteIOGen,&interfazGenerica.unidades_de_trabajo,sizeof(int));
					agregar_a_paquete(paqueteIOGen,&interfazGenerica.PID,sizeof(int));
					agregar_a_paquete(paqueteIOGen,interfazGenerica.nombre_interfaz,pathLength);					
					enviar_paquete(paqueteIOGen,socketCliente);
					eliminar_paquete(paqueteIOGen);
					
					//bloquear procesos? //TODO
				}
				else{
					PCB* proceso = cambiarAExitDesdeRunning(lista_RUNNING);
					paquete_memoria_finalizar_proceso(proceso->PID);
					//eliminarProceso(proceso); //TODO
				}
				free(interfazGenerica.nombre_interfaz);
				break;
			}
			case IO_STDIN_READ:
			{
				/*procesoCPU = recibirProcesoContextoEjecucion(stream);
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "RR") || !strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					terminarHiloQuantum();
				}
				pthread_mutex_lock(&mutexListaRunning);
				pthread_mutex_lock(&mutexListaBlocked);
				procesoKernel = list_remove(lista_RUNNING, 0);
				actualizarProceso(procesoCPU, procesoKernel);
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					temporal_stop(tiempoVRR);
					tiempoEjecutando = temporal_gettime(tiempoVRR);
					temporal_destroy(tiempoVRR);
					procesoKernel->quantum -= tiempoEjecutando;
				}
				procesoKernel->estado = BLOCKED;
				list_add(lista_BLOCKED, procesoKernel);
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaBlocked);
				sem_post(&semListaRunning);
				log_info(loggerKernel, "El proceso con pid:%d se interrumpio por IO_GEN_SLEEP\n", procesoKernel->PID);
				*/ 
				//ESTO ESTA COPYPASTEADO NO SI ESTA BN
				Peticion_Interfaz_STDIN interfazsSTDIN;

				PCB* proceso;

				int pathLength;
				
				
				memcpy(&interfazsSTDIN.direccion, stream, sizeof(uint32_t));
				stream += sizeof(uint32_t);
				memcpy(&interfazsSTDIN.tamanio, stream, sizeof(uint8_t));
				stream += sizeof(uint8_t);
				memcpy(&interfazsSTDIN.PID, stream, sizeof(int));
				stream += sizeof(int);
				memcpy(&pathLength, stream, sizeof(int));
				interfazsSTDIN.nombre_interfaz = malloc(pathLength);
				memcpy(interfazsSTDIN.nombre_interfaz, stream, pathLength);

				//MUTEX
				if(existeInterfaz(interfazsSTDIN.nombre_interfaz)){
					t_paquete* paqueteIOSTDIN=crear_paquete(IO_GEN_SLEEP);
					agregar_a_paquete(paqueteIOSTDIN,&interfazsSTDIN.direccion,sizeof(uint32_t));
					agregar_a_paquete(paqueteIOSTDIN,&interfazsSTDIN.tamanio,sizeof(uint8_t));
					agregar_a_paquete(paqueteIOSTDIN,&interfazsSTDIN.PID,sizeof(int));
					agregar_a_paquete(paqueteIOSTDIN,interfazsSTDIN.nombre_interfaz,pathLength);					
					enviar_paquete(paqueteIOSTDIN,socketCliente);
					eliminar_paquete(paqueteIOSTDIN);
					
					//bloquear procesos? //TODO
				}
				else{
					PCB* proceso = cambiarAExitDesdeRunning(lista_RUNNING);
					paquete_memoria_finalizar_proceso(proceso->PID);
					//eliminarProceso(proceso); //TODO
				}
				free(interfazsSTDIN.nombre_interfaz);

				break;
			}
			case IO_STDOUT_WRITE:
			{
				/*procesoCPU = recibirProcesoContextoEjecucion(stream);
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "RR") || !strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					terminarHiloQuantum();
				}
				pthread_mutex_lock(&mutexListaRunning);
				pthread_mutex_lock(&mutexListaBlocked);
				procesoKernel = list_remove(lista_RUNNING, 0);
				actualizarProceso(procesoCPU, procesoKernel);
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					temporal_stop(tiempoVRR);
					tiempoEjecutando = temporal_gettime(tiempoVRR);
					temporal_destroy(tiempoVRR);
					procesoKernel->quantum -= tiempoEjecutando;
				}
				procesoKernel->estado = BLOCKED;
				list_add(lista_BLOCKED, procesoKernel);
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaBlocked);
				sem_post(&semListaRunning);
				log_info(loggerKernel, "El proceso con pid:%d se interrumpio por IO_GEN_SLEEP\n", procesoKernel->PID);
				*/ 
				//ESTO ESTA COPYPASTEADO NO SI ESTA BN
				Peticion_Interfaz_STDOUT interfazsSTDOUT;

				PCB* proceso;

				int pathLength;
				
				
				memcpy(&interfazsSTDOUT.direccion, stream, sizeof(uint32_t));
				stream += sizeof(uint32_t);
				memcpy(&interfazsSTDOUT.tamanio, stream, sizeof(uint8_t));
				stream += sizeof(uint8_t);
				memcpy(&interfazsSTDOUT.PID, stream, sizeof(int));
				stream += sizeof(int);
				memcpy(&pathLength, stream, sizeof(int));
				interfazsSTDOUT.nombre_interfaz = malloc(pathLength);
				memcpy(interfazsSTDOUT.nombre_interfaz, stream, pathLength);

				//MUTEX
				if(existeInterfaz(interfazsSTDOUT.nombre_interfaz)){
					t_paquete* paqueteIOSTDOUT=crear_paquete(IO_GEN_SLEEP);
					agregar_a_paquete(paqueteIOSTDOUT,&interfazsSTDOUT.direccion,sizeof(uint32_t));
					agregar_a_paquete(paqueteIOSTDOUT,&interfazsSTDOUT.tamanio,sizeof(uint8_t));
					agregar_a_paquete(paqueteIOSTDOUT,&interfazsSTDOUT.PID,sizeof(int));
					agregar_a_paquete(paqueteIOSTDOUT,interfazsSTDOUT.nombre_interfaz,pathLength);					
					enviar_paquete(paqueteIOSTDOUT,socketCliente);
					eliminar_paquete(paqueteIOSTDOUT);
					
					//bloquear procesos? //TODO
				}
				else{
					PCB* proceso = cambiarAExitDesdeRunning(lista_RUNNING);
					paquete_memoria_finalizar_proceso(proceso->PID);
					//eliminarProceso(proceso); //TODO
				}
				free(interfazsSTDOUT.nombre_interfaz);

				break;
			}
			case IO_FS_CREATE:
			{
				break;
			}
			case IO_FS_DELETE:
			{
				break;
			}
			case IO_FS_TRUNCATE:
			{
				break;
			}
			case IO_FS_WRITE:
			{
				break;
			}
			case IO_FS_READ:
			{
				break;
			}
				
			default:
			{
				//ningun mensaje valido recibido
				break;
			}
			eliminar_paquete(paquete);
		}
	}
}

int existeInterfaz(char *nombre){
	//mutex lista
	Interfaces_conectadas_kernel *interfazBuffer;
	for (int i = 0; i < list_size(interfacesConectadas); i++)
	{
		interfazBuffer=list_get(interfacesConectadas, i);
		if(!strcmp(interfazBuffer->nombre, nombre)){
			return 1;
		}

	}
	return 0;
}

void* atenderPeticionesIO() {
    while (1) {
        int socketCliente = esperarClienteV2(loggerKernel, server_fd);
        pthread_t client_thread;
        int* pclient = malloc(sizeof(int));
        *pclient = socketCliente;
        pthread_create(&client_thread, NULL, manejarClienteIO, pclient);
        pthread_detach(client_thread);
    }
    return NULL;
}

void* manejarClienteIO(void *arg)
{
    int socketCliente = *((int*)arg);
    free(arg);
    while(1){
        t_paquete* paquete = malloc(sizeof(t_paquete));
        paquete->buffer = malloc(sizeof(t_buffer));

        
        recv(socketCliente, &(paquete->codigo_operacion), sizeof(op_code), 0);
        recv(socketCliente, &(paquete->buffer->size), sizeof(int), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
		void *stream = paquete->buffer->stream;
		switch(paquete->codigo_operacion){
        	case AGREGAR_INTERFACES:
            {
				Interfaces_conectadas_kernel *interfazBuffer = malloc(sizeof(Interfaces_conectadas_kernel));
				int charTam;
				memcpy(&charTam, stream, sizeof(int));
				stream += sizeof(int);
				interfazBuffer->nombre = malloc(charTam);
				memcpy(&interfazBuffer->nombre,stream, sizeof(charTam));
				stream += charTam;
				memcpy(&interfazBuffer->tipo, stream , sizeof(Tipos_Interfaz));

				list_add(interfacesConectadas, interfazBuffer);

				log_info(loggerKernel, "Se conecto y guardo la interfaz con nombre:%s",interfazBuffer->nombre);
				
				break;
			}
			case TERMINO_INTERFAZ:
			{
				//recibo nombre de la interfaz para sacarlo de la lista y pasarlo de bloqueado a ready
			}
			case ERROR_EN_INTERFAZ:
			{
				break;
			}
			default:
			{
				//ningun mensaje valido recibido
				break;
			}
		}
	}
}

////////////////////////////////////////////////////////// EMPAQUETACION //////////////////////////////////////////////////////////

PCB *recibirProcesoContextoEjecucion(void *stream){
	PCB* proceso = malloc(sizeof(PCB));
	proceso->estado = RUNNING;
	memcpy(&proceso->PID, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&proceso->cpuRegisters.PC, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&proceso->cpuRegisters.AX, stream, sizeof(uint8_t));
	stream += sizeof(uint8_t);
	memcpy(&proceso->cpuRegisters.BX, stream, sizeof(uint8_t));
	stream += sizeof(uint8_t);
	memcpy(&proceso->cpuRegisters.CX, stream, sizeof(uint8_t));
	stream += sizeof(uint8_t);
	memcpy(&proceso->cpuRegisters.DX, stream, sizeof(uint8_t));
	stream += sizeof(uint8_t);
	memcpy(&proceso->cpuRegisters.EAX, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&proceso->cpuRegisters.EBX, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&proceso->cpuRegisters.ECX, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&proceso->cpuRegisters.EDX, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&proceso->cpuRegisters.SI, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(&proceso->cpuRegisters.DI, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);

	return proceso;
}

void paquete_CPU_ejecutar_proceso(PCB* proceso){
    t_paquete *paquete_CPU = crear_paquete(EJECUTAR_PROCESO);

    agregar_entero_a_paquete32(paquete_CPU, proceso->PID);
    agregar_entero_a_paquete32(paquete_CPU, proceso->cpuRegisters.PC);
    agregar_entero_a_paquete8(paquete_CPU, proceso->cpuRegisters.AX);
    agregar_entero_a_paquete8(paquete_CPU, proceso->cpuRegisters.BX);
    agregar_entero_a_paquete8(paquete_CPU, proceso->cpuRegisters.CX);
    agregar_entero_a_paquete8(paquete_CPU, proceso->cpuRegisters.DX);
    agregar_entero_a_paquete32(paquete_CPU, proceso->cpuRegisters.EAX);
    agregar_entero_a_paquete32(paquete_CPU, proceso->cpuRegisters.EBX);
    agregar_entero_a_paquete32(paquete_CPU, proceso->cpuRegisters.ECX);
    agregar_entero_a_paquete32(paquete_CPU, proceso->cpuRegisters.EDX);
    agregar_entero_a_paquete32(paquete_CPU, proceso->cpuRegisters.SI);
    agregar_entero_a_paquete32(paquete_CPU, proceso->cpuRegisters.DI);

    enviar_paquete(paquete_CPU, cpu_dispatch_fd);
    eliminar_paquete(paquete_CPU);
}

void paquete_CPU_interrumpir_proceso_fin_quantum(int pid){
    t_paquete *paquete_CPU = crear_paquete(INTERRUMPIR_PROCESO);

    agregar_entero_a_paquete32(paquete_CPU, pid);
    enviar_paquete(paquete_CPU, cpu_interrupt_fd);
    eliminar_paquete(paquete_CPU);
}

void InterruptACPU(){
    t_paquete *paquete_CPU_interrupcion = crear_paquete(INTERRUMPIR_PROCESO);

    enviar_paquete(paquete_CPU_interrupcion, cpu_interrupt_fd);
    eliminar_paquete(paquete_CPU_interrupcion);
}

// MANEJO DE RECURSOS

void wait_recurso(t_configKernel *config, char *nombre_recurso, int pid) {
    int recurso_index = -1;
    for (int i = 0; config->RECURSOS[i] != NULL; i++) {
        if (strcmp(config->RECURSOS[i], nombre_recurso) == 0) {
            recurso_index = i;
            break;
        }
    }

    if (recurso_index == -1) {
        // El recurso no existe, terminar el proceso
        printf("Recurso %s no encontrado. Terminando proceso %d\n", nombre_recurso, pid);
        // EXIT
        return;
    }

    if (config->INSTANCIAS_RECURSOS[recurso_index] <= 0) {
        // No hay instancias disponibles, terminar el proceso
        printf("No hay instancias disponibles para el recurso %s. Terminando proceso %d\n", nombre_recurso, pid);
        // EXIT
        return;
    }

    // Disminuir la instancia del recurso
    config->INSTANCIAS_RECURSOS[recurso_index]--;
    printf("Proceso %d hizo WAIT en el recurso %s. Instancias restantes: %d\n", pid, nombre_recurso, config->INSTANCIAS_RECURSOS[recurso_index]);
}

void signal_recurso(t_configKernel *config, char *nombre_recurso, int pid) {
    int recurso_index = -1;
    for (int i = 0; i < config->num_recursos; i++) {
        if (strcmp(config->RECURSOS[i], nombre_recurso) == 0) {
            recurso_index = i;
            break;
        }
    }

    if (recurso_index == -1) {
        // El recurso no existe, terminar el proceso
        printf("Recurso %s no encontrado. Terminando proceso %d\n", nombre_recurso, pid);
        // Aquí debes incluir la lógica para terminar el proceso
        return;
    }

    // Aumentar la instancia del recurso
    config->INSTANCIAS_RECURSOS[recurso_index]++;
    printf("Proceso %d hizo SIGNAL en el recurso %s. Instancias restantes: %d\n", pid, nombre_recurso, config->INSTANCIAS_RECURSOS[recurso_index]);
}