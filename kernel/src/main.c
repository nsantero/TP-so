#include <utils.h>
#include <kernel.h>
#include <conexiones.h>
#include <consola.h>
#include <configs.h>
#include <planificadores.h>

int main(int argc, char *argv[]) {
    
	iniciarLogger();
    armarConfig();

    inicializarListas();
    inicializar_sem_planificadores();

    //CONEXIONES HACER EN ARCHIVO APARTE PARA DESPEJAR MAIN

	// Conecto kernel con cpu y memoria
	cpu_dispatch_fd = crear_conexion(loggerKernel,"CPU_DISPATCH",configuracionKernel.IP_CPU,configuracionKernel.PUERTO_CPU_DISPATCH);
	//log_info(logger, "Me conecte a cpu (dispatch)");
    cpu_interrupt_fd = crear_conexion(loggerKernel,"CPU_INTERRUPT",configuracionKernel.IP_CPU,configuracionKernel.PUERTO_CPU_INTERRUPT);
	log_info(loggerKernel, "Me conecte a cpu (interrupt)");
    memoria_fd = crear_conexion(loggerKernel,"MEMORIA",configuracionKernel.IP_MEMORIA,configuracionKernel.PUERTO_MEMORIA);
	log_info(loggerKernel, "Me conecte a memoria");

	// envio mensajes
	enviar_mensaje("soy Kernel", memoria_fd);
    enviar_mensaje("soy Kernel", cpu_dispatch_fd);

	//levanto servidor
	server_fd = iniciar_servidor(loggerKernel,server_name ,IP, configuracionKernel.PUERTO_ESCUCHA);
	log_info(loggerKernel, "Servidor listo para recibir al cliente");

    pthread_t hiloConsola, procesosNew, procesosReady;
    pthread_create(&hiloConsola,NULL, manejadorDeConsola, NULL);
    pthread_create(&procesosNew,NULL, planificadorNew, NULL);
    pthread_create(&procesosReady,NULL, planificadorReady, NULL);

    pthread_join(hiloConsola, NULL);
    pthread_join(procesosNew, NULL);
    pthread_join(procesosReady, NULL);

    return 0;
}
