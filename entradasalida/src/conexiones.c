#include <conexiones.h>


int kernel_fd;
int memoria_fd;

void enviarNuevaInterfazAKernel(t_config* configNueva,char* nombre){

    int tamanioNombre=string_length(nombre);
    Tipos_Interfaz tipoDeLaInterfaz=obtenerTipoConString(config_get_string_value(configNueva,"TIPO_INTERFAZ"));

    t_paquete* paquete=crear_paquete(AGREGAR_INTERFACES);
    agregar_a_paquete(paquete,tamanioNombre,sizeof(int));
    agregar_a_paquete(paquete,nombre,tamanioNombre);
    agregar_a_paquete(paquete,tipoDeLaInterfaz,sizeof(Tipos_Interfaz));
    enviar_paquete(paquete,kernel_fd);
    eliminar_paquete(paquete);

}