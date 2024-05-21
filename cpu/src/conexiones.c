#include "../include/conexiones.h"

static void procesar_conexion_dispatch(void* void_args) {
    int *args = (int*) void_args;
        int cliente_socket_dispatch = *args;

    op_code cop;
        while (cliente_socket_dispatch != -1 && !recibio_interrupcion) {

        if (recv(cliente_socket_dispatch, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, ANSI_COLOR_BLUE"El cliente se desconecto de DISPATCH");
            return;
        }

        switch (cop) {
            case MENSAJE:
                char* mensaje = recibir_mensaje(cliente_socket_dispatch);
                log_info(logger, "Recibi el mensaje: %s , soy dispatch", mensaje);
                break;

            default: {
                log_error(logger, "Código de operación no reconocido en Dispatch: %d", cop);
                break;
            }
        }

    }

    return;
}

static void procesar_conexion_interrupt(void* void_args) {
    int *args = (int*) void_args;
        int cliente_socket_interrupt = *args;

    op_code cop;

    while (cliente_socket_interrupt != -1) {

        if (recv(cliente_socket_interrupt, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, ANSI_COLOR_BLUE"El cliente se desconecto de INTERRUPT");
            return;
        }

        switch (cop) {
            case MENSAJE:
                char* mensaje = recibir_mensaje(cliente_socket_interrupt);
                    log_info(logger, "Recibi el mensaje: %s , soy interrupt", mensaje);
                break;
            default: {
                log_error(logger, "Código de operación no reconocido en Interrupt: %d", cop);
                break;
            }
        }
    }    
}
