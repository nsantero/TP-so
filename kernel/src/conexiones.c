#include <conexiones.h>
#include <configs.h>

int memoria_fd;
int cpu_dispatch_fd=0;
int cpu_interrupt_fd=0;
int server_fd;
char* server_name = "kernel";
int socketCliente;

//TODO mover esto a donde este comodo
uint32_t leerValorDelRegistro(char *registroAleer,CPU_Registers registros){
	if (strcmp(registroAleer,"PC")==0){
		return registros.PC;
	}else if (strcmp(registroAleer,"AX")==0){
		return registros.AX;
	}else if (strcmp(registroAleer,"BX")==0){
		return registros.BX;
	}else if (strcmp(registroAleer,"CX")==0){
		return registros.CX;
	}else if (strcmp(registroAleer,"DC")==0){
		return registros.DX;
	}else if (strcmp(registroAleer,"EAX")==0){
		return registros.EAX;
	}else if (strcmp(registroAleer,"EBX")==0){
		return registros.EBX;
	}else if (strcmp(registroAleer,"ECX")==0){
		return registros.ECX;
	}else if (strcmp(registroAleer,"EDX")==0){
		return registros.EDX;
	}else if (strcmp(registroAleer,"SI")==0){
		return registros.SI;
	}else if (strcmp(registroAleer,"DI")==0){
		return registros.DI;
	}
	return 0;
}

////////////////////////////////////////////////////////// PROCESO CONEXION //////////////////////////////////////////////////////////
t_paquete* recibirPaquete(int socket){
	t_paquete* paquete = NULL;
	paquete = malloc(sizeof(t_paquete));
	paquete->buffer = NULL;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->codigo_operacion = 0;
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;

	recv(socket, &(paquete->codigo_operacion), sizeof(op_code), 0);
	recv(socket, &(paquete->buffer->size), sizeof(int), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);

	return paquete;
}

void* conexionesDispatch()
{
	
	while (1)
	{
		t_paquete* paquete=recibirPaquete(cpu_dispatch_fd);
		void *stream = paquete->buffer->stream;
		PCB *procesoCPU;
		PCB* procesoKernel;
		switch (paquete->codigo_operacion)
		{
			case PROCESO_EXIT:
			{
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				pthread_mutex_lock(&mutexListaRunning);
				pthread_mutex_lock(&mutexListaExit);
				procesoKernel = list_remove(lista_RUNNING, 0);
				if(procesoCPU->PID == procesoKernel->PID){
					actualizarProceso(procesoCPU, procesoKernel);
				}
				else{
					pthread_mutex_lock(&mutexLogger);
					log_error(loggerKernel,"los procesos que se quieren actualizar son distintos el de CPU:%d, Kernel:%d",procesoCPU->PID, procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(procesoCPU);
				procesoKernel->estado = EXIT;
				list_add(lista_EXIT, procesoKernel); 
				//proceso = cambiarAExitDesdeRunning(lista_RUNNING);
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaExit);
				sem_post(&semListaRunning);
				//mutex conexion memoria
				paquete_memoria_finalizar_proceso(procesoKernel->PID);

				pthread_mutex_lock(&mutexLogger);
				log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <SUCCESS>\n", procesoKernel->PID);
				pthread_mutex_unlock(&mutexLogger);

				//eliminarProceso(procesoKernel);
				break;
			}
			case INTERRUMPIR_PROCESO:
			{
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				pthread_mutex_lock(&mutexListaRunning);
				pthread_mutex_lock(&mutexListaExit);
				procesoKernel = list_remove(lista_RUNNING, 0);
				if(procesoCPU->PID == procesoKernel->PID){
					actualizarProceso(procesoCPU, procesoKernel);
				}
				else{
					log_error(loggerKernel,"los procesos que se quieren actualizar son distintos el de CPU:%d, Kernel:%d",procesoCPU->PID, procesoKernel->PID);

				}
				free(procesoCPU);
				procesoKernel->estado = EXIT;
				list_add(lista_EXIT, procesoKernel); 
				//proceso = cambiarAExitDesdeRunning(lista_RUNNING);
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaExit);
				sem_post(&semListaRunning);
				//mutex conexion memoria
				paquete_memoria_finalizar_proceso(procesoKernel->PID);
				break;

			}
			case PROCESO_INTERRUMPIDO_CLOCK:
			{
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				
				pthread_mutex_lock(&mutexListaReady);
				pthread_mutex_lock(&mutexListaRunning);
				sem_wait(&semPlaniReadyClock);
				sem_post(&semPlaniReadyClock);
				procesoKernel = list_remove(lista_RUNNING, 0);
				pthread_mutex_lock(&mutexLogger);
				log_info(loggerKernel, "PID: %d - Desalojado por fin Quantum", procesoKernel->PID);
				pthread_mutex_unlock(&mutexLogger);
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					procesoKernel->quantum=configuracionKernel.QUANTUM;
				}
				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "RR") || !strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
					actualizarProceso(procesoCPU, procesoKernel);
					procesoKernel->estado = READY;
					list_add(lista_READY, procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <READY>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(procesoCPU);
				
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaReady);
				sem_post(&semListaReady);
				sem_post(&semListaRunning);
				break;
			}
			case PROCESO_WAIT:
			{
				char* recursoRecibido=recibirRecurso(stream);
				Recurso *recursoEncontrado = buscarRecurso(recursoRecibido);
				// Si no encuentro el recurso -- Finaliza el proceso ok
	 			if (recursoEncontrado == NULL) {
					//enviarMensaje("RECURSO NO ENCONTRADO", cpu_dispatch_fd);
					t_paquete* paquete2=recibirPaquete(cpu_dispatch_fd);
					void *stream2 = paquete2->buffer->stream;

					procesoCPU = recibirProcesoContextoEjecucion(stream2);
					pthread_mutex_lock(&mutexListaRunning);
					pthread_mutex_lock(&mutexListaExit);
					procesoKernel = list_remove(lista_RUNNING, 0);

					if(procesoCPU->PID == procesoKernel->PID){
						actualizarProceso(procesoCPU, procesoKernel);
					}
					else{
						log_error(loggerKernel,"los procesos que se quieren actualizar son distintos el de CPU:%d, Kernel:%d",procesoCPU->PID, procesoKernel->PID);
					}
					free(procesoCPU);
					procesoKernel->estado = EXIT;
					list_add(lista_EXIT, procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_RESOURCE:%s>", procesoKernel->PID, recursoRecibido);
					pthread_mutex_unlock(&mutexLogger);

					pthread_mutex_unlock(&mutexListaRunning);
					pthread_mutex_unlock(&mutexListaExit);
					sem_post(&semListaRunning);
					paquete_memoria_finalizar_proceso(procesoKernel->PID);

   				}

				// Si el recurso existe y tiene instancias
				if (recursoEncontrado->instancias > 0) {
					recursoEncontrado->instancias--;
					enviar_resultado_recursos(WAIT_SUCCESS, cpu_dispatch_fd);
					pthread_mutex_lock(&mutexListaRunning);
					procesoKernel = list_get(lista_RUNNING, 0);
					list_add(procesoKernel->recursosEnUso,recursoEncontrado);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel,"Proceso %d pudo utilizar recurso %s.",procesoKernel->PID, recursoEncontrado->nombre);
					pthread_mutex_unlock(&mutexLogger);

					pthread_mutex_unlock(&mutexListaRunning);
				} 

				// Si el recurso existe pero no hay instancias del mismo se bloquea el proceso
				else {
					recursoEncontrado->instancias--;
					enviar_resultado_recursos(WAIT_BLOCK, cpu_dispatch_fd);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: <%d> - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel,"PID: <%d> - Bloqueado por: <%s>", procesoKernel->PID, recursoEncontrado->nombre);
					pthread_mutex_unlock(&mutexLogger);
					bloquearProcesoRecurso(procesoCPU,procesoKernel, recursoEncontrado->nombre);
					
					
				}
				free(recursoRecibido);
				break;
			}
			case PROCESO_SIGNAL:
			{
				char* recursoRecibido=recibirRecurso(stream);
				Recurso *recursoEncontrado = buscarRecurso(recursoRecibido);
				// Si no encuentro el recurso -- Finaliza el proceso ok
	 			if (recursoEncontrado == NULL) {
					//enviarMensaje("RECURSO NO ENCONTRADO", cpu_dispatch_fd);

					t_paquete* paquete2=recibirPaquete(cpu_dispatch_fd);
					void *stream2 = paquete2->buffer->stream;

					procesoCPU = recibirProcesoContextoEjecucion(stream2);
					pthread_mutex_lock(&mutexListaRunning);
					pthread_mutex_lock(&mutexListaExit);
					procesoKernel = list_remove(lista_RUNNING, 0);

					if(procesoCPU->PID == procesoKernel->PID){
						actualizarProceso(procesoCPU, procesoKernel);
					}
					else{
						log_error(loggerKernel,"los procesos que se quieren actualizar son distintos el de CPU:%d, Kernel:%d",procesoCPU->PID, procesoKernel->PID);
					}
					free(procesoCPU);
					procesoKernel->estado = EXIT;
					list_add(lista_EXIT, procesoKernel); 
					pthread_mutex_unlock(&mutexListaRunning);
					pthread_mutex_unlock(&mutexListaExit);
					sem_post(&semListaRunning);
					paquete_memoria_finalizar_proceso(procesoKernel->PID);

					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel,"Recurso %s no encontrado. Terminando proceso %d", recursoRecibido, procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);

   				}
				// Si el recurso existe
				if (recursoEncontrado != NULL) {
					recursoEncontrado->instancias++;
					// busco si hay algún proceso esperando el recurso
					for (int i=0; i<list_size(lista_BLOCKED_RECURSOS); i++) {
						PCB *procesoBloqueado = list_get(lista_BLOCKED_RECURSOS, i);
						if (strcmp(procesoBloqueado->recursoBloqueante, recursoEncontrado->nombre) == 0) {
							// lo muevo a ready
							pthread_mutex_lock(&mutexListaReady);
							pthread_mutex_lock(&mutexListaBlockedRecursos);
							procesoKernel = list_remove(lista_BLOCKED_RECURSOS, i);
							procesoKernel->estado = READY;
							procesoKernel->recursoBloqueante = NULL;
							list_add(lista_READY, procesoKernel);
							pthread_mutex_unlock(&mutexListaBlockedRecursos);
							pthread_mutex_unlock(&mutexListaReady);
							sem_post(&semListaReady);
							pthread_mutex_lock(&mutexLogger);
							log_info(loggerKernel,"Proceso %d desbloqueado por señal de recurso %s", procesoKernel->PID, recursoEncontrado->nombre);
							pthread_mutex_unlock(&mutexLogger);
						}

						
					}
					enviar_resultado_recursos(SIGNAL_SUCCESS, cpu_dispatch_fd);
				}
				free(recursoRecibido);
				break;
			}
			case RESIZE_ERROR:
			{
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				pthread_mutex_lock(&mutexListaRunning);
				pthread_mutex_lock(&mutexListaExit);
				procesoKernel = list_remove(lista_RUNNING, 0);
				if(procesoCPU->PID == procesoKernel->PID){
					actualizarProceso(procesoCPU, procesoKernel);
				}
				else{
					log_error(loggerKernel,"los procesos que se quieren actualizar son distintos el de CPU:%d, Kernel:%d",procesoCPU->PID, procesoKernel->PID);

				}
				free(procesoCPU);
				procesoKernel->estado = EXIT;
				list_add(lista_EXIT, procesoKernel); 
				//proceso = cambiarAExitDesdeRunning(lista_RUNNING);
				pthread_mutex_unlock(&mutexListaRunning);
				pthread_mutex_unlock(&mutexListaExit);
				sem_post(&semListaRunning);
				//mutex conexion memoria
				paquete_memoria_finalizar_proceso(procesoKernel->PID);
				pthread_mutex_lock(&mutexLogger);
				log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <OUT_OF_MEMORY>", procesoKernel->PID);
				pthread_mutex_unlock(&mutexLogger);
				break;
			}
			// INSTRUCCIONES I/O
			case IO_GEN_SLEEP:
			{
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				stream += 8*sizeof(uint32_t) + 4*sizeof(uint8_t); // como recibe mas cosas se suma el el registri DI
				sem_wait(&semPlaniBlocked);
				sem_post(&semPlaniBlocked);
				procesoKernel = desalojarProceso(procesoKernel, procesoCPU);
				free(procesoCPU);

				Peticion_Interfaz_Generica interfazGenerica;
				Tipos_Interfaz tipoDeInterfazEncontrada;
				int pathLength;
				
				memcpy(&pathLength, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				interfazGenerica.nombre_interfaz = malloc(pathLength);
				memcpy(interfazGenerica.nombre_interfaz, stream, pathLength);
				stream+=pathLength;
				memcpy(&interfazGenerica.unidades_de_trabajo, stream, sizeof(uint32_t));
				
				interfazGenerica.PID=procesoKernel->PID;
				//MUTEX
				int socketClienteInterfaz = existeInterfaz(interfazGenerica.nombre_interfaz,&tipoDeInterfazEncontrada);
				if (tipoDeInterfazEncontrada!=T_GENERICA){
					//error no es una intruccion compatible //TODO
				}
				if(socketClienteInterfaz){
					t_paquete* paqueteIOGen=crear_paquete(IO_GEN_SLEEP);
					agregar_a_paquete(paqueteIOGen,&interfazGenerica.unidades_de_trabajo,sizeof(int));
					agregar_a_paquete(paqueteIOGen,&interfazGenerica.PID,sizeof(int));
					agregar_a_paquete(paqueteIOGen,interfazGenerica.nombre_interfaz,pathLength);					
					enviar_paquete(paqueteIOGen,socketClienteInterfaz);
					eliminar_paquete(paqueteIOGen);
					bloquearProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel, "PID: %d - Bloqueado por: <%s>", procesoKernel->PID, interfazGenerica.nombre_interfaz);
					pthread_mutex_unlock(&mutexLogger);
				}
				else{
					terminarProceso(procesoKernel); //TODO falta pasarle la lista para bloquearla
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_INTERFACE>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(interfazGenerica.nombre_interfaz);
				break;
			}

			case IO_STDIN_READ:
			{
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				stream += 8*sizeof(uint32_t) + 4*sizeof(uint8_t); // como recibe mas cosas se suma el el registri DI
				procesoKernel = desalojarProceso(procesoKernel, procesoCPU);
				free(procesoCPU);
				Peticion_Interfaz_STDIN interfazsSTDIN;
				Tipos_Interfaz tipoDeInterfazEncontrada;
				int pathLength;
				uint32_t cantPags;
				uint32_t frameAux;
				

				memcpy(&pathLength, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				interfazsSTDIN.nombre_interfaz = malloc(pathLength);
				memcpy(interfazsSTDIN.nombre_interfaz, stream, pathLength);
				stream+=pathLength;
				//primer registro
				memcpy(&interfazsSTDIN.direccion_fisica.bytes, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&interfazsSTDIN.direccion_fisica.desplazamiento, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&interfazsSTDIN.direccion_fisica.numero_frame, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				//segundoRegistro
				memcpy(&interfazsSTDIN.tamanio, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&interfazsSTDIN.tamPag, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&cantPags, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);

				interfazsSTDIN.PID=procesoKernel->PID;

				//MUTEX
				int socketClienteInterfaz = existeInterfaz(interfazsSTDIN.nombre_interfaz,&tipoDeInterfazEncontrada);
				if (tipoDeInterfazEncontrada!=T_STDIN){
					//error no es una intruccion compatible //TODO
				}
				
				if(socketClienteInterfaz){
					t_paquete* paqueteIOSTDIN=crear_paquete(IO_STDIN_READ);
					agregar_entero_a_paquete32(paqueteIOSTDIN,interfazsSTDIN.direccion_fisica.bytes);
					agregar_entero_a_paquete32(paqueteIOSTDIN,interfazsSTDIN.direccion_fisica.desplazamiento);
					agregar_entero_a_paquete32(paqueteIOSTDIN,interfazsSTDIN.direccion_fisica.numero_frame);
					agregar_entero_a_paquete32(paqueteIOSTDIN,interfazsSTDIN.tamanio);
					agregar_entero_a_paquete32(paqueteIOSTDIN,interfazsSTDIN.PID);
					agregar_a_paquete(paqueteIOSTDIN,interfazsSTDIN.nombre_interfaz,pathLength);
					agregar_entero_a_paquete32(paqueteIOSTDIN,interfazsSTDIN.tamPag);
					agregar_entero_a_paquete32(paqueteIOSTDIN, cantPags);
					for(int i =1; i<cantPags; i++){
						memcpy(&frameAux, stream, sizeof(uint32_t));
						stream+=sizeof(uint32_t);
						agregar_entero_a_paquete32(paqueteIOSTDIN,frameAux);
					}				
					enviar_paquete(paqueteIOSTDIN,socketClienteInterfaz);
					eliminar_paquete(paqueteIOSTDIN);
					bloquearProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel, "PID: %d - Bloqueado por: <%s>", procesoKernel->PID, interfazsSTDIN.nombre_interfaz);
					pthread_mutex_unlock(&mutexLogger);
				}
				else{
					terminarProceso(procesoKernel);
					//eliminarProceso(proceso); //TODO
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_INTERFACE>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(interfazsSTDIN.nombre_interfaz);

				break;
			}
			case IO_STDOUT_WRITE:
			{
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				stream += 8*sizeof(uint32_t) + 4*sizeof(uint8_t); // como recibe mas cosas se suma el el registri DI
				sem_wait(&semPlaniBlocked);
				sem_post(&semPlaniBlocked);
				procesoKernel = desalojarProceso(procesoKernel, procesoCPU);
				free(procesoCPU);

				Peticion_Interfaz_STDOUT peticionSTDOUT;
				Tipos_Interfaz tipoDeInterfazEncontrada;
				int pathLength;
				uint32_t cantPags;
				uint32_t frameAux;
				
				memcpy(&pathLength, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionSTDOUT.nombre_interfaz = malloc(pathLength);
				memcpy(peticionSTDOUT.nombre_interfaz, stream, pathLength);
				stream+=pathLength;
				//primer registro
				memcpy(&peticionSTDOUT.direccion_fisica.bytes, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionSTDOUT.direccion_fisica.desplazamiento, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionSTDOUT.direccion_fisica.numero_frame, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				//segundoRegistro
				memcpy(&peticionSTDOUT.tamanio, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionSTDOUT.tamPag, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&cantPags, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				


				peticionSTDOUT.PID=procesoKernel->PID;
				//MUTEX
				int socketClienteInterfaz = existeInterfaz(peticionSTDOUT.nombre_interfaz,&tipoDeInterfazEncontrada);
				if (tipoDeInterfazEncontrada!=T_STDOUT){
					//error no es una intruccion compatible //TODO
				}
				if(socketClienteInterfaz){
					t_paquete* paqueteSTDOUT=crear_paquete(IO_STDOUT_WRITE);
					agregar_entero_a_paquete32(paqueteSTDOUT,peticionSTDOUT.direccion_fisica.bytes);
					agregar_entero_a_paquete32(paqueteSTDOUT,peticionSTDOUT.direccion_fisica.desplazamiento);
					agregar_entero_a_paquete32(paqueteSTDOUT,peticionSTDOUT.direccion_fisica.numero_frame);
					agregar_entero_a_paquete32(paqueteSTDOUT,peticionSTDOUT.tamanio);
					agregar_entero_a_paquete32(paqueteSTDOUT,peticionSTDOUT.PID);
					agregar_a_paquete(paqueteSTDOUT,peticionSTDOUT.nombre_interfaz,pathLength);
					agregar_entero_a_paquete32(paqueteSTDOUT,peticionSTDOUT.tamPag);
					agregar_entero_a_paquete32(paqueteSTDOUT, cantPags);
					for(int i =1; i<cantPags; i++){
						memcpy(&frameAux, stream, sizeof(uint32_t));
						stream+=sizeof(uint32_t);
						agregar_entero_a_paquete32(paqueteSTDOUT,frameAux);
					}
					enviar_paquete(paqueteSTDOUT, socketClienteInterfaz);
					eliminar_paquete(paqueteSTDOUT);
					bloquearProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel, "PID: %d - Bloqueado por: <%s>", procesoKernel->PID, peticionSTDOUT.nombre_interfaz);
					pthread_mutex_unlock(&mutexLogger);
				}
				else{
					terminarProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_INTERFACE>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(peticionSTDOUT.nombre_interfaz);
				

				break;
			}
			case IO_FS_CREATE:
			{
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				stream += 8*sizeof(uint32_t) + 4*sizeof(uint8_t); // como recibe mas cosas se suma el el registri DI
				sem_wait(&semPlaniBlocked);
				sem_post(&semPlaniBlocked);
				procesoKernel = desalojarProceso(procesoKernel, procesoCPU);
				free(procesoCPU);

				Peticion_Interfaz_DialFS peticionFS;
				Tipos_Interfaz tipoDeInterfazEncontrada;
				int pathLength;
				int archivoLen;
				
				memcpy(&pathLength, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombre_interfaz = malloc(pathLength);
				memcpy(peticionFS.nombre_interfaz, stream, pathLength);
				stream+=pathLength;
				
				memcpy(&archivoLen, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombreArchivo=malloc(archivoLen);
				memcpy(peticionFS.nombreArchivo,stream,archivoLen);

				peticionFS.PID=procesoKernel->PID;
				peticionFS.operacion=DFS_CREATE;
				//MUTEX
				int socketClienteInterfaz = existeInterfaz(peticionFS.nombre_interfaz,&tipoDeInterfazEncontrada);
				if (tipoDeInterfazEncontrada!=T_DFS){
					//error no es una intruccion compatible //TODO
				}
				if(socketClienteInterfaz){
					t_paquete* paqueteFS=crear_paquete(IO_FS_CREATE);
					agregar_a_paquete(paqueteFS,&peticionFS.operacion,sizeof(OperacionesDeDialFS));
					agregar_entero_a_paquete32(paqueteFS, archivoLen);
    				agregar_string_a_paquete(paqueteFS, peticionFS.nombreArchivo);
					agregar_entero_a_paquete32(paqueteFS,peticionFS.PID);
					agregar_a_paquete(paqueteFS,peticionFS.nombre_interfaz,pathLength);					
					enviar_paquete(paqueteFS,socketClienteInterfaz);
					eliminar_paquete(paqueteFS);
					bloquearProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel, "PID: %d - Bloqueado por: <%s>", procesoKernel->PID, peticionFS.nombre_interfaz);
					pthread_mutex_unlock(&mutexLogger);
				}
				else{
					terminarProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_INTERFACE>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(peticionFS.nombre_interfaz);
				free(peticionFS.nombreArchivo);
				

				break;
			}
			case IO_FS_DELETE:
			{
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				stream += 8*sizeof(uint32_t) + 4*sizeof(uint8_t); // como recibe mas cosas se suma el el registri DI
				sem_wait(&semPlaniBlocked);
				sem_post(&semPlaniBlocked);
				procesoKernel = desalojarProceso(procesoKernel, procesoCPU);
				free(procesoCPU);

				Peticion_Interfaz_DialFS peticionFS;
				Tipos_Interfaz tipoDeInterfazEncontrada;
				int pathLength;
				int archivoLen;
				
				memcpy(&pathLength, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombre_interfaz = malloc(pathLength);
				memcpy(peticionFS.nombre_interfaz, stream, pathLength);
				stream+=pathLength;
				
				memcpy(&archivoLen, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombreArchivo=malloc(archivoLen);
				memcpy(peticionFS.nombreArchivo,stream,archivoLen);

				peticionFS.PID=procesoKernel->PID;
				peticionFS.operacion=DFS_DELETE;
				//MUTEX
				int socketClienteInterfaz = existeInterfaz(peticionFS.nombre_interfaz,&tipoDeInterfazEncontrada);
				if (tipoDeInterfazEncontrada!=T_DFS){
					//error no es una intruccion compatible //TODO
				}
				if(socketClienteInterfaz){
					t_paquete* paqueteFS=crear_paquete(IO_FS_DELETE);
					agregar_a_paquete(paqueteFS,&peticionFS.operacion,sizeof(OperacionesDeDialFS));
					agregar_entero_a_paquete32(paqueteFS, archivoLen);
    				agregar_string_a_paquete(paqueteFS, peticionFS.nombreArchivo);
					agregar_entero_a_paquete32(paqueteFS,peticionFS.PID);
					agregar_a_paquete(paqueteFS,peticionFS.nombre_interfaz,pathLength);					
					enviar_paquete(paqueteFS,socketClienteInterfaz);
					eliminar_paquete(paqueteFS);
					bloquearProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel, "PID: %d - Bloqueado por: <%s>", procesoKernel->PID, peticionFS.nombre_interfaz);
					pthread_mutex_unlock(&mutexLogger);
				}
				else{
					terminarProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_INTERFACE>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(peticionFS.nombre_interfaz);
				free(peticionFS.nombreArchivo);
				

				break;
			}
			case IO_FS_TRUNCATE:
			{
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				stream += 8*sizeof(uint32_t) + 4*sizeof(uint8_t); // como recibe mas cosas se suma el el registri DI
				sem_wait(&semPlaniBlocked);
				sem_post(&semPlaniBlocked);
				procesoKernel = desalojarProceso(procesoKernel, procesoCPU);
				free(procesoCPU);

				Peticion_Interfaz_DialFS peticionFS;
				Tipos_Interfaz tipoDeInterfazEncontrada;
				int pathLength;
				int archivoLen;
				
				memcpy(&pathLength, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombre_interfaz = malloc(pathLength);
				memcpy(peticionFS.nombre_interfaz, stream, pathLength);
				stream+=pathLength;
				
				memcpy(&archivoLen, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombreArchivo=malloc(archivoLen);
				memcpy(peticionFS.nombreArchivo,stream,archivoLen);
				stream+=archivoLen;
				memcpy(&peticionFS.tamanio,stream,sizeof(uint32_t));

				peticionFS.PID=procesoKernel->PID;
				peticionFS.operacion=DFS_TRUNCATE;
				//MUTEX
				int socketClienteInterfaz = existeInterfaz(peticionFS.nombre_interfaz,&tipoDeInterfazEncontrada);
				if (tipoDeInterfazEncontrada!=T_DFS){
					//error no es una intruccion compatible //TODO
				}
				if(socketClienteInterfaz){
					t_paquete* paqueteFS=crear_paquete(IO_FS_TRUNCATE);
					agregar_a_paquete(paqueteFS,&peticionFS.operacion,sizeof(OperacionesDeDialFS));
					agregar_entero_a_paquete32(paqueteFS, archivoLen);
    				agregar_string_a_paquete(paqueteFS, peticionFS.nombreArchivo);
					agregar_entero_a_paquete32(paqueteFS,peticionFS.tamanio);
					agregar_entero_a_paquete32(paqueteFS,peticionFS.PID);
					agregar_a_paquete(paqueteFS,peticionFS.nombre_interfaz,pathLength);					
					enviar_paquete(paqueteFS,socketClienteInterfaz);
					eliminar_paquete(paqueteFS);
					bloquearProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel, "PID: %d - Bloqueado por: <%s>", procesoKernel->PID, peticionFS.nombre_interfaz);
					pthread_mutex_unlock(&mutexLogger);
				}
				else{
					terminarProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_INTERFACE>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(peticionFS.nombre_interfaz);
				free(peticionFS.nombreArchivo);
				

				break;
			}
			case IO_FS_WRITE:
			{
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				stream += 8*sizeof(uint32_t) + 4*sizeof(uint8_t); // como recibe mas cosas se suma el el registri DI
				sem_wait(&semPlaniBlocked);
				sem_post(&semPlaniBlocked);
				procesoKernel = desalojarProceso(procesoKernel, procesoCPU);
				free(procesoCPU);

				Peticion_Interfaz_DialFS peticionFS;
				Tipos_Interfaz tipoDeInterfazEncontrada;
				int pathLength;
				int archivoLen;
				uint32_t cantPags;
				uint32_t frameAux;
				
				memcpy(&pathLength, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombre_interfaz = malloc(pathLength);
				memcpy(peticionFS.nombre_interfaz, stream, pathLength);
				stream+=pathLength;
				
				memcpy(&archivoLen, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombreArchivo=malloc(archivoLen);
				memcpy(peticionFS.nombreArchivo,stream,archivoLen);
				stream+=archivoLen;
				memcpy(&peticionFS.Direccion_fisica.bytes,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionFS.Direccion_fisica.desplazamiento,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionFS.Direccion_fisica.numero_frame,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionFS.tamanio,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionFS.punteroArchivo,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&cantPags, stream, sizeof(uint32_t));

				peticionFS.PID=procesoKernel->PID;
				peticionFS.operacion=DFS_WRITE;
				//MUTEX
				int socketClienteInterfaz = existeInterfaz(peticionFS.nombre_interfaz,&tipoDeInterfazEncontrada);
				if (tipoDeInterfazEncontrada!=T_DFS){
					//error no es una intruccion compatible //TODO
				}
				if(socketClienteInterfaz){
					t_paquete* paqueteFS=crear_paquete(IO_FS_WRITE);
					agregar_a_paquete(paqueteFS,&peticionFS.operacion,sizeof(OperacionesDeDialFS));
					agregar_entero_a_paquete32(paqueteFS, archivoLen);
    				agregar_string_a_paquete(paqueteFS, peticionFS.nombreArchivo);
					agregar_entero_a_paquete32(paqueteFS, peticionFS.Direccion_fisica.bytes);
					agregar_entero_a_paquete32(paqueteFS, peticionFS.Direccion_fisica.desplazamiento);
					agregar_entero_a_paquete32(paqueteFS, peticionFS.Direccion_fisica.numero_frame);
					agregar_entero_a_paquete32(paqueteFS,peticionFS.tamanio);
					agregar_entero_a_paquete32(paqueteFS, peticionFS.punteroArchivo);
					agregar_entero_a_paquete32(paqueteFS,peticionFS.PID);
					agregar_a_paquete(paqueteFS,peticionFS.nombre_interfaz,pathLength);	
					agregar_entero_a_paquete32(paqueteFS,cantPags);
					for(int i =1; i<cantPags; i++){
						memcpy(&frameAux, stream, sizeof(uint32_t));
						stream+=sizeof(uint32_t);
						agregar_entero_a_paquete32(paqueteFS,frameAux);
					}				
					enviar_paquete(paqueteFS,socketClienteInterfaz);
					eliminar_paquete(paqueteFS);
					bloquearProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel, "PID: %d - Bloqueado por: <%s>", procesoKernel->PID, peticionFS.nombre_interfaz);
					pthread_mutex_unlock(&mutexLogger);
				}
				else{
					terminarProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_INTERFACE>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(peticionFS.nombre_interfaz);
				free(peticionFS.nombreArchivo);
				

				break;
			}
			case IO_FS_READ:
			{
				sem_wait(&semPlaniRunning);
				sem_post(&semPlaniRunning);
				procesoCPU = recibirProcesoContextoEjecucion(stream);
				stream += 8*sizeof(uint32_t) + 4*sizeof(uint8_t); // como recibe mas cosas se suma el el registri DI
				sem_wait(&semPlaniBlocked);
				sem_post(&semPlaniBlocked);
				procesoKernel = desalojarProceso(procesoKernel, procesoCPU);
				free(procesoCPU);

				Peticion_Interfaz_DialFS peticionFS;
				Tipos_Interfaz tipoDeInterfazEncontrada;
				int pathLength;
				int archivoLen;
				
				uint32_t cantPags;
				uint32_t frameAux;
				
				memcpy(&pathLength, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombre_interfaz = malloc(pathLength);
				memcpy(peticionFS.nombre_interfaz, stream, pathLength);
				stream+=pathLength;
				
				memcpy(&archivoLen, stream, sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				peticionFS.nombreArchivo=malloc(archivoLen);
				memcpy(peticionFS.nombreArchivo,stream,archivoLen);
				stream+=archivoLen;
				memcpy(&peticionFS.Direccion_fisica.bytes,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionFS.Direccion_fisica.desplazamiento,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionFS.Direccion_fisica.numero_frame,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionFS.tamanio,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&peticionFS.punteroArchivo,stream,sizeof(uint32_t));
				stream+=sizeof(uint32_t);
				memcpy(&cantPags, stream, sizeof(uint32_t));
				peticionFS.PID=procesoKernel->PID;
				peticionFS.operacion=DFS_READ;
				//MUTEX
				int socketClienteInterfaz = existeInterfaz(peticionFS.nombre_interfaz,&tipoDeInterfazEncontrada);
				if (tipoDeInterfazEncontrada!=T_DFS){
					//error no es una intruccion compatible //TODO
				}		
				if(socketClienteInterfaz){
					t_paquete* paqueteFS=crear_paquete(IO_FS_READ);
					agregar_a_paquete(paqueteFS,&peticionFS.operacion,sizeof(OperacionesDeDialFS));
					agregar_entero_a_paquete32(paqueteFS, archivoLen);
    				agregar_string_a_paquete(paqueteFS, peticionFS.nombreArchivo);
					agregar_entero_a_paquete32(paqueteFS, peticionFS.Direccion_fisica.bytes);
					agregar_entero_a_paquete32(paqueteFS, peticionFS.Direccion_fisica.desplazamiento);
					agregar_entero_a_paquete32(paqueteFS, peticionFS.Direccion_fisica.numero_frame);
					agregar_entero_a_paquete32(paqueteFS,peticionFS.tamanio);
					agregar_entero_a_paquete32(paqueteFS, peticionFS.punteroArchivo);
					agregar_entero_a_paquete32(paqueteFS,peticionFS.PID);
					agregar_a_paquete(paqueteFS,peticionFS.nombre_interfaz,pathLength);					
					agregar_entero_a_paquete32(paqueteFS,cantPags);
					for(int i =1; i<cantPags; i++){
						memcpy(&frameAux, stream, sizeof(uint32_t));
						stream+=sizeof(uint32_t);
						agregar_entero_a_paquete32(paqueteFS,frameAux);
					}	
					enviar_paquete(paqueteFS,socketClienteInterfaz);
					eliminar_paquete(paqueteFS);
					bloquearProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "PID: %d - Estado Anterior: <RUNNING> - Estado Actual: <BLOCKED>", procesoKernel->PID);
					log_info(loggerKernel, "PID: %d - Bloqueado por: <%s>", procesoKernel->PID, peticionFS.nombre_interfaz);
					pthread_mutex_unlock(&mutexLogger);
				}
				else{
					terminarProceso(procesoKernel);
					pthread_mutex_lock(&mutexLogger);
					log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <INVALID_INTERFACE>", procesoKernel->PID);
					pthread_mutex_unlock(&mutexLogger);
				}
				free(peticionFS.nombre_interfaz);
				free(peticionFS.nombreArchivo);
				

				break;
			}
				
			default:
			{
				//ningun mensaje valido recibido
				break;
			}
			eliminar_paquete(paquete);
		}
		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);
	}
}
char* recibirRecurso(void* stream){
	int recursoLength;
	char *recursoRecibido;
	memcpy(&recursoLength, stream, sizeof(int));
	stream += sizeof(int);
	recursoRecibido = malloc(recursoLength);
	memcpy(recursoRecibido, stream, recursoLength);

	return recursoRecibido;
}
Recurso* buscarRecurso(char* recursoRecibido){
	Recurso *recursoEncontrado = NULL;
	for (int i = 0; i < list_size(configuracionKernel.RECURSOS); i++) {
		Recurso *recursoActual = list_get(configuracionKernel.RECURSOS, i);
		if (strcmp(recursoActual->nombre, recursoRecibido) == 0) {
			recursoEncontrado = recursoActual;
			break;
		}
	}
	return recursoEncontrado;
}
void bloquearProcesoRecurso(PCB* procesoCPU, PCB* procesoKernel, char* recurso){
	t_paquete* paquete=recibirPaquete(cpu_dispatch_fd);
	void *stream = paquete->buffer->stream;
	enviar_resultado_recursos(WAIT_BLOCK, cpu_dispatch_fd);
	procesoCPU = recibirProcesoContextoEjecucion(stream);
	pthread_mutex_lock(&mutexListaBlockedRecursos);
	pthread_mutex_lock(&mutexListaRunning);
	procesoKernel = list_remove(lista_RUNNING, 0);
	actualizarProceso(procesoCPU, procesoKernel);
	free(procesoCPU);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	procesoKernel->recursoBloqueante = recurso;
	procesoKernel->estado = BLOCKED;
	list_add(lista_BLOCKED_RECURSOS, procesoKernel);
	pthread_mutex_unlock(&mutexListaRunning);
	pthread_mutex_unlock(&mutexListaBlockedRecursos);
	sem_post(&semListaRunning);
}

int existeInterfaz(char *nombre,Tipos_Interfaz* tipo){
	//mutex lista
	Interfaces_conectadas_kernel *interfazBuffer;
	if(list_size(interfacesConectadas) != 0){
		for (int i = 0; i < list_size(interfacesConectadas); i++)
		{
			interfazBuffer=list_get(interfacesConectadas, i);
			if(!strcmp(interfazBuffer->nombre, nombre)){
				*tipo= interfazBuffer->tipo;
				return interfazBuffer->socketCliente;
			}
		}
	}
	
	return 0;
}
PCB* procesoBloqueado(uint32_t pid){
	pthread_mutex_lock(&mutexListaBlocked);
	PCB *proceso;
	if(list_size(lista_BLOCKED) != 0){
		for (int i = 0; i < list_size(lista_BLOCKED); i++)
		{
			proceso=list_get(lista_BLOCKED, i);
			if(proceso->PID == pid){
				proceso = list_remove(lista_BLOCKED, i);
				pthread_mutex_unlock(&mutexListaBlocked);
				return proceso;
			}
		}
	}
	pthread_mutex_unlock(&mutexListaBlocked);
	return NULL;
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
        t_paquete* paquete=recibirPaquete(socketCliente);
		void *stream = paquete->buffer->stream;
		switch(paquete->codigo_operacion){
        	case AGREGAR_INTERFACES:
            {
				Interfaces_conectadas_kernel *interfazBuffer = malloc(sizeof(Interfaces_conectadas_kernel));
				int charTam;
				memcpy(&charTam, stream, sizeof(int));
				stream += sizeof(int);
				interfazBuffer->nombre = malloc(charTam);
				memcpy(interfazBuffer->nombre,stream, charTam);
				stream += charTam;
				stream += sizeof(int);
				memcpy(&interfazBuffer->tipo, stream , sizeof(Tipos_Interfaz));
				interfazBuffer->socketCliente = socketCliente;
				list_add(interfacesConectadas, interfazBuffer);
				//TODO agregar q si se vuelve a conectar una interfaz
				pthread_mutex_lock(&mutexLogger);
				log_info(loggerKernel, "Se conecto y guardo la interfaz con nombre:%s",interfazBuffer->nombre);
				pthread_mutex_unlock(&mutexLogger);
				
				break;
			}
			case DESBLOQUEAR_PROCESO_POR_IO:
			{
				//recibo nombre de la interfaz para sacarlo de la lista y pasarlo de bloqueado a ready
				//char *nombre;
				int charTam;
				uint32_t pid;
				PCB* proceso =NULL;
				memcpy(&charTam, stream, sizeof(int));
				stream += sizeof(int);
				stream += charTam;
				//nombre no lo necesito
				memcpy(&pid,stream,sizeof(uint32_t));
				proceso=procesoBloqueado(pid);
				sem_wait(&semPlaniBlocked);
				sem_post(&semPlaniBlocked);
				/*if(proceso == NULL){
					//logger
				}*/

				if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR"))
				{
					if(proceso->quantum < configuracionKernel.QUANTUM && proceso->quantum > 0){
						proceso->estado = READY;
						pthread_mutex_lock(&mutexListaReadyPri);
						list_add(lista_READYPRI, proceso);
						pthread_mutex_unlock(&mutexListaReadyPri);
						sem_post(&semListaReady);
					}
					else{
						proceso->quantum=configuracionKernel.QUANTUM;
						proceso->estado = READY;
						pthread_mutex_lock(&mutexListaReady);
						list_add(lista_READY, proceso);
						pthread_mutex_unlock(&mutexListaReady);
						sem_post(&semListaReady);
					}
					
				}
				else{
					proceso->estado = READY;
					pthread_mutex_lock(&mutexListaReady);
					list_add(lista_READY, proceso);
					pthread_mutex_unlock(&mutexListaReady);
					sem_post(&semListaReady);
				}
				pthread_mutex_lock(&mutexLogger);
				log_info(loggerKernel,"PID: %d - Estado Anterior: <BLOCKED> - Estado Actual: <READY>", proceso->PID);
				pthread_mutex_unlock(&mutexLogger);
				break;
			}
			case ERROR_EN_INTERFAZ:
			{
				char *nombre;
				int charTam;
				uint32_t pid;
				PCB* proceso =NULL;
				memcpy(&charTam, stream, sizeof(int));
				stream += sizeof(int);
				nombre = malloc(charTam);
				memcpy(nombre,stream, charTam);
				stream += charTam;

				memcpy(&pid,stream,sizeof(uint32_t));
				pthread_mutex_lock(&mutexListaExit);
				proceso=procesoBloqueado(pid);
				proceso->estado = EXIT;
				list_add(lista_EXIT, proceso); 
				pthread_mutex_unlock(&mutexListaExit);
				//mutex conexion memoria
				paquete_memoria_finalizar_proceso(proceso->PID);
				pthread_mutex_lock(&mutexLogger);
				log_info(loggerKernel, "PID: %d - Estado Anterior: BLOCKED> - Estado Actual: <EXIT>", proceso->PID);
				log_info(loggerKernel, "Finaliza el proceso %d - Motivo: <ERROR_DE_INTERFAZ:%s>", proceso->PID, nombre);
				pthread_mutex_unlock(&mutexLogger);

				break;
			}
			default:
			{
				//ningun mensaje valido recibido
				break;
			}
		
		}
		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);
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
    t_paquete *paquete_CPU = crear_paquete(PROCESO_INTERRUMPIDO_CLOCK);

    agregar_entero_a_paquete32(paquete_CPU, pid);
    enviar_paquete(paquete_CPU, cpu_interrupt_fd);
    eliminar_paquete(paquete_CPU);
}

void InterruptACPU(){
    t_paquete *paquete_CPU_interrupcion = crear_paquete(INTERRUMPIR_PROCESO);

    enviar_paquete(paquete_CPU_interrupcion, cpu_interrupt_fd);
    eliminar_paquete(paquete_CPU_interrupcion);
}


void enviar_resultado_recursos(op_code resultadoRecursos,int socket_cliente){

    t_paquete *paquete_cpu_recursos = crear_paquete(resultadoRecursos);
    
    enviar_paquete(paquete_cpu_recursos, socket_cliente);

    eliminar_paquete(paquete_cpu_recursos);
}

PCB* desalojarProceso(PCB* procesoKernel, PCB* procesoCPU){
	if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "RR") || !strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
		terminarHiloQuantum();
	}
	pthread_mutex_lock(&mutexListaRunning);
	procesoKernel = list_remove(lista_RUNNING, 0);
	actualizarProceso(procesoCPU, procesoKernel);
	if(!strcmp(configuracionKernel.ALGORITMO_PLANIFICACION, "VRR")){
		procesoKernel->quantum -= tiempoEjecutando;
	}
	pthread_mutex_unlock(&mutexListaRunning);
	sem_post(&semListaRunning);

	return procesoKernel;
}

void bloquearProceso(PCB* proceso){
	pthread_mutex_lock(&mutexListaBlocked);
	proceso->estado = BLOCKED;
	list_add(lista_BLOCKED, proceso);
	pthread_mutex_unlock(&mutexListaBlocked);
}

void terminarProceso(PCB* proceso){
	paquete_memoria_finalizar_proceso(proceso->PID);
	proceso->estado = EXIT;
	pthread_mutex_lock(&mutexListaExit);
	list_add(lista_EXIT, proceso);
	pthread_mutex_unlock(&mutexListaExit);
}