#include <utils.h>
#include <kernel.h>
#include <conexiones.h>
#include <consola.h>
#include <configs.h>
#include <planificadores.h>
#include <signal.h>
void handleSiginitKernel(){
    close(cpu_dispatch_fd);
    close(cpu_interrupt_fd);
    close(memoria_fd);
    close(server_fd);
}

int main(int argc, char *argv[]) {

    signal(SIGINT,handleSiginitKernel);
    
	iniciarLogger();
    armarConfig();
    inicializarListas();
    inicializar_sem_planificadores();

    //CONEXIONES HACER EN ARCHIVO APARTE PARA DESPEJAR MAIN

	// Conecto kernel con cpu y memoria
    
	cpu_dispatch_fd = crear_conexion(loggerKernel,"CPU_DISPATCH",configuracionKernel.IP_CPU,configuracionKernel.PUERTO_CPU_DISPATCH);
	log_info(loggerKernel, "Me conecte a cpu (dispatch)");
    cpu_interrupt_fd = crear_conexion(loggerKernel,"CPU_INTERRUPT",configuracionKernel.IP_CPU,configuracionKernel.PUERTO_CPU_INTERRUPT);
	log_info(loggerKernel, "Me conecte a cpu (interrupt)");
    
    memoria_fd = crear_conexion(loggerKernel,"MEMORIA",configuracionKernel.IP_MEMORIA,configuracionKernel.PUERTO_MEMORIA);
	log_info(loggerKernel, "Me conecte a memoria");

	// envio mensajes
	//enviar_mensaje("soy Kernel", memoria_fd);
    enviar_mensaje("soy Kernel", cpu_dispatch_fd);

	//levanto servidor
    char * server_name = "Kernel";
	server_fd = iniciar_servidor(loggerKernel,server_name, configuracionKernel.PUERTO_ESCUCHA);
	log_info(loggerKernel, "Servidor listo para recibir al cliente");

    pthread_t hiloConsola, procesosNew, procesosReady, hiloDispatch, hiloInterfaz;
    pthread_create(&hiloConsola,NULL, manejadorDeConsola, NULL);
    pthread_create(&procesosNew,NULL, planificadorNew, NULL);
    pthread_create(&procesosReady,NULL, planificadorReady, NULL);
	pthread_create(&hiloDispatch, NULL, conexionesDispatch, NULL);
    pthread_create(&hiloInterfaz,NULL, atenderPeticionesIO, NULL);

    pthread_detach(procesosNew);
    pthread_detach(procesosReady);
    pthread_detach(hiloDispatch);
    pthread_detach(hiloInterfaz);
    pthread_join(hiloConsola, NULL);

    return 0;
}
