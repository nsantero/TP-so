#include <memoria.h>
#include <conexiones.h>
#include <semaforos.h>
#include <signal.h>
#include <math.h>

Memoria memoria;
t_list *lista_ProcesosActivos=NULL;
int seguirCorriendo = 1;

void crearListas(){

    lista_ProcesosActivos = list_create(); 

}

void esquemaPaginacion(){
    
    int cant_frames=0;
    int calculo_espacio_frames=0;
    int cant_frames_bitarray=0;
    void* espacio_frames=NULL;

    cant_frames = memoria.tam/memoria.pagina_tam;

    calculo_espacio_frames = ceil((double)cant_frames/8);

    espacio_frames =malloc(calculo_espacio_frames);
    
    memoria.bitmap_frames =  bitarray_create_with_mode(espacio_frames,calculo_espacio_frames,LSB_FIRST);

    cant_frames_bitarray = bitarray_get_max_bit(memoria.bitmap_frames);

    //int diferencia = cant_frames_bitarray-memoria.cantidad_frames;

    for (int i = 0; i < cant_frames_bitarray; i++) {
    
        bitarray_clean_bit(memoria.bitmap_frames, i);
        
    }
    
    //log_info(loggerMemoria, "Se creo el esquema de frames");

}

int calculoDeFrames(int memoria_tam, int pagina_tam){

    return memoria_tam/pagina_tam;

}

void inicializarMemoria(){

    memoria.tam = configuracionMemoria.TAM_MEMORIA;
    memoria.pagina_tam = configuracionMemoria.TAM_PAGINA;
    memoria.cantidad_frames = calculoDeFrames(memoria.tam, memoria.pagina_tam);
    memoria.espacioUsuario = malloc(memoria.tam);
    pthread_mutex_lock(&actualizarLoggerMemoria);
    log_info(loggerMemoria, "Memoria inicializada con las configuraciones");
    pthread_mutex_unlock(&actualizarLoggerMemoria);

   
}

void handleSiginitMemoria(){
    seguirCorriendo = 0;
    close(server_fd);
    exit(0);
}


int main(int argc, char *argv[])
{
    signal(SIGINT,handleSiginitMemoria);
    
    iniciarLoggerMemoria();

	armarConfigMemoria();
    
    crearListas();

    inicializarMemoria();
    esquemaPaginacion();
    
    server_fd = iniciarServidorV2(loggerMemoria, configuracionMemoria.PUERTO_ESCUCHA);
    
    pthread_mutex_lock(&actualizarLoggerMemoria);
	log_info(loggerMemoria, "Servidor listo para recibir al cliente");
    pthread_mutex_unlock(&actualizarLoggerMemoria);

    /*pthread_t hiloCpu;
    pthread_create(&hiloCpu, NULL, atenderPeticionesCpu, NULL);*/
    pthread_t hiloKernel;
    pthread_create(&hiloKernel, NULL, atenderPeticionesKernel, NULL);

    //pthread_t hiloEntradaSalida;
    //pthread_create(&hiloEntradaSalida, NULL, atenderPeticionesEntradaSalida, NULL);

    //pthread_detach(hiloCpu);
    pthread_join(hiloKernel, NULL);
    //pthread_detach(hiloEntradaSalida);

    
    
    pthread_mutex_destroy(&listaProcesosActivos);
    pthread_mutex_destroy(&accesoAMemoria);
    pthread_mutex_destroy(&accesoBitArray);
    pthread_mutex_destroy(&actualizarLoggerMemoria);

    close(server_fd);

    return 0;
}

