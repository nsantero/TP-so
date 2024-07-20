#include <memoria.h>
#include <conexiones.h>
#include <semaforos.h>
#include <signal.h>

Memoria memoria;
t_list *lista_ProcesosActivos=NULL;

void crearListas(){

    lista_ProcesosActivos = list_create(); 

}

void esquemaPaginacion(){
    
    int cant_frames = memoria.tam/memoria.pagina_tam;
    int calculo_espacio_frames = ceil((double)cant_frames/8);
    void* espacio_frames =malloc(calculo_espacio_frames);
    
    memoria.bitmap_frames =  bitarray_create_with_mode(espacio_frames,calculo_espacio_frames,LSB_FIRST);

    int cant_frames_bitarray = bitarray_get_max_bit(memoria.bitmap_frames);
    int diferencia = cant_frames_bitarray-memoria.cantidad_frames;

    for (int i = 0; i < cant_frames_bitarray; i++) {
    
        bitarray_clean_bit(memoria.bitmap_frames, i);
        
    }
    
    log_info(loggerMemoria, "Se creo el esquema de frames");

}

int calculoDeFrames(int memoria_tam, int pagina_tam){

    return memoria_tam/pagina_tam;

}

void inicializarMemoria(){
    
    memoria.tam = configuracionMemoria.TAM_MEMORIA;
    memoria.pagina_tam = configuracionMemoria.TAM_PAGINA;
    memoria.cantidad_frames = calculoDeFrames(memoria.tam, memoria.pagina_tam);
    memoria.espacioUsuario = malloc(memoria.tam);

    log_info(loggerMemoria, "Memoria inicializada con las configuraciones");
}

void handleSiginitMemoria(){
    close(server_fd);
}


int main(int argc, char *argv[])
{
    signal(SIGINT,handleSiginitMemoria);
    
    iniciarLoggerMemoria();

    log_info(loggerMemoria, "Servidor Memoria listo para recibir al cliente");

	armarConfigMemoria();
    
    crearListas();

    Memoria *memoria = malloc(sizeof(Memoria));

    inicializarMemoria();
    esquemaPaginacion();
    
    server_fd = iniciarServidorV2(loggerMemoria, configuracionMemoria.PUERTO_ESCUCHA);
	log_info(loggerMemoria, "Servidor listo para recibir al cliente");

    pthread_t hiloCpu;
    pthread_create(&hiloCpu, NULL, atenderPeticionesCpu, NULL);

    pthread_t hiloKernel;
    pthread_create(&hiloKernel, NULL, atenderPeticionesKernel, NULL);

    pthread_t hiloEntradaSalida;
    pthread_create(&hiloEntradaSalida, NULL, atenderPeticionesEntradaSalida, NULL);

    pthread_detach(hiloCpu);
    pthread_detach(hiloKernel);
    pthread_detach(hiloEntradaSalida);


    while(1){
    }
    
    close(server_fd);

    return 0;
}

