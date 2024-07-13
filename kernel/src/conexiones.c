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
				//mutex conexion memoria
				paquete_memoria_finalizar_proceso(procesoKernel->PID);

				log_info(loggerKernel, "Se elimino el proceso con pid: %d\n", procesoKernel->PID);

				eliminarProceso(procesoKernel);
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
				}
				else{
					log_error(loggerKernel,"Solo se tienen que interrumpir los procesos que usan RR o VRR");
				}
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaReady);

				log_info(loggerKernel, "El proceso con pid:%d se interrumpio por fin de qunatum\n", procesoKernel->PID);
				break;
			}
			case PROCESO_WAIT:
			{
				
				break;
			}
			case PROCESO_SIGNAL:
			{
				break;
			}
			case IO_GEN_SLEEP:
			{
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
					enviar_paquete(paquete, socketCliente);
				}
				else{
					PCB* proceso = cambiarAExitDesdeRunning(lista_RUNNING);
					paquete_memoria_finalizar_proceso(proceso->PID);
					//eliminarProceso(proceso); //TODO
				}
				
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
				Interfaces_conectadas_kernel *interfazBuffer;
				int cantidadDeInterfaces;
				int charTam;
				memcpy(&charTam, stream, sizeof(int));
				stream += sizeof(int);
				interfazBuffer->nombre = malloc(charTam);
				memcpy(&interfazBuffer->nombre,stream, sizeof(charTam));
				stream += charTam;
				memcpy(&interfazBuffer->tipo, stream , sizeof(Tipos_Interfaz));

				interfazBuffer->ocupada = 0;
				interfazBuffer->solicitudesEnCola = 0;
				interfazBuffer->pidActual = -1;

				list_add(interfacesConectadas, interfazBuffer);

				log_info(loggerKernel, "Se conecto y guardo la interfaz con nombre:%s",interfazBuffer->nombre);
				
				break;
			}
			case TERMINO_INTERFAZ:
			{
				//recibo nombre de la interfaz para sacarlo de la lista y pasarlo de bloqueado a ready
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
	PCB* proceso;
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


