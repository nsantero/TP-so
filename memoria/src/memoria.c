#include <memoria.h>
#include <conexiones.h>
#include <semaforos.h>

Memoria memoria;
t_list *lista_frames;
t_list *lista_de_paginas_proceso;
t_list *lista_ProcesosActivos=NULL;


void crearListas(){

    lista_ProcesosActivos = list_create(); 
    lista_frames = list_create(); 
    lista_de_paginas_proceso = list_create(); 

}

void esquemaPaginacion(){
    
    int calculo_espacio_frames = memoria.cantidad_frames/8 +1 ;
    void* espacio_frames =malloc(calculo_espacio_frames);
     
    memoria.bitmap_frames =  bitarray_create_with_mode(espacio_frames,calculo_espacio_frames,LSB_FIRST);
    
}

int calculoDeFrames(int memoria_tam, int pagina_tam){

    return memoria_tam/pagina_tam;

}

void inicializarMemoria(){

    Memoria *memoria = malloc(sizeof(Memoria));
    
    memoria->tam = configuracionMemoria.TAM_MEMORIA;
    memoria->pagina_tam = configuracionMemoria.TAM_PAGINA;
    memoria->cantidad_frames = calculoDeFrames(memoria->tam, memoria->pagina_tam);
    memoria->espacioUsuario = malloc(memoria->tam);

}


int main(int argc, char *argv[])
{
    
    printf("Modulo Memoria\n");

    iniciarLoggerMemoria();
	armarConfigMemoria();
    crearListas();
    inicializarMemoria();
    esquemaPaginacion();
    
    server_fd = iniciarServidorV2(loggerMemoria, configuracionMemoria.PUERTO_ESCUCHA);
    
	log_info(loggerMemoria, "Servidor listo para recibir al cliente");
    pthread_t hiloCpu;
    pthread_create(&hiloCpu, NULL, atenderPeticionesCpu, NULL);

    pthread_t hiloKernel;
    pthread_create(&hiloKernel, NULL, atenderPeticionesKernel, NULL);

    pthread_detach(hiloCpu);
    pthread_detach(hiloKernel);


    while(1){
    }
    
    close(server_fd);

    return 0;
}

