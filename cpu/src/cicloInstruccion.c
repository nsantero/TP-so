#include <cicloInstruccion.h>
#include <stdio.h> 

extern int memoria_fd; 
extern int program_counter; 

/*
void fetch() {
    char mensaje[50]; 

    // Preparar el mensaje con el valor actual del PC
    sprintf(mensaje, "Dame la instrucción número %d", program_counter);

    // Enviar mensaje a memoria solicitando la instrucción
    enviar_mensaje(mensaje, memoria_fd);

    // Recibir la instrucción de memoria y almacenarla de alguna manera
    recv_instruccion(memoria_fd);

    // Actualizar el Program Counter
    program_counter += 1;
}

void setRegistro(char *nombre, int valor) {
    // Implementar lógica para asignar un valor a un registro
    printf("Registro %s actualizado con valor %d\n", nombre, valor);
}

void decode_execute() {
    // Asumiendo que instruccionActual ya ha sido llenado con la instrucción y operandos correctos
    if (strcmp(instruccionActual.instruccion, "SUM") == 0) {
        // Convertir operandos a enteros
        int op1 = atoi(instruccionActual.operando1);
        int op2 = atoi(instruccionActual.operando2);
        // Realizar la suma
        int resultado = op1 + op2;
        // Se debería actualizar el registro o variable donde se almacena el resultado
        printf("Resultado de SUM: %d\n", resultado);
    }
        else if (strcmp(instruccionActual.instruccion, "SET") == 0) {
            // Asigna al registro el valor pasado como parámetro.
            int valor = atoi(instruccionActual.operando2);
            setRegistro(instruccionActual.operando1, valor);
        }
        else if (strcmp(instruccionActual.instruccion, "MOV_IN") == 0) {
            // Lee el valor de memoria y lo almacena en el Registro Datos.
            // Asumiendo que existe una función getValorMemoria(direccion) y setRegistro(nombre, valor)
            int direccion = atoi(instruccionActual.operando2);
            int valor = getValorMemoria(direccion);
            setRegistro(instruccionActual.operando1, valor);
        }
        else if (strcmp(instruccionActual.instruccion, "MOV_OUT") == 0) {
            // Lee el valor del Registro Datos y lo escribe en la memoria.
            // Asumiendo que existe una función getRegistro(nombre) y setValorMemoria(direccion, valor)
            int valor = getRegistro(instruccionActual.operando2);
            int direccion = atoi(instruccionActual.operando1);
            setValorMemoria(direccion, valor);
        }
        else if (strcmp(instruccionActual.instruccion, "SUB") == 0) {
            // Resta al Registro Destino el Registro Origen.
            int valorOrigen = getRegistro(instruccionActual.operando2);
            int valorDestino = getRegistro(instruccionActual.operando1);
            setRegistro(instruccionActual.operando1, valorDestino - valorOrigen);
        }
        else if (strcmp(instruccionActual.instruccion, "JNZ") == 0) {
            // Si el valor del registro es distinto de cero, actualiza el program counter.
            int valorRegistro = getRegistro(instruccionActual.operando1);
            if (valorRegistro != 0) {
                setProgramCounter(atoi(instruccionActual.operando2));
            }
        }
        
        else if (strcmp(instruccionActual.instruccion, "RESIZE") == 0) {
            // Solicitar ajuste de tamaño de proceso. Manejar respuesta de memoria.
            int nuevoTamaño = atoi(instruccionActual.operando1);
            if (!resizeMemoria(nuevoTamaño)) {
                // Enviar contexto de ejecución al Kernel por Out of Memory.
                enviarContextoEjecucionAlKernel();
            }
        }
        else if (strcmp(instruccionActual.instruccion, "COPY_STRING") == 0) {
            // Copia string de SI a la posición de memoria DI.
            copiarString(atoi(instruccionActual.operando1));
        }
        else if (strcmp(instruccionActual.instruccion, "WAIT") == 0) {
            // Solicita al Kernel asignar una instancia del recurso.
            solicitarRecursoAlKernel(instruccionActual.operando1);
        }
        else if (strcmp(instruccionActual.instruccion, "SIGNAL") == 0) {
            // Solicita al Kernel liberar una instancia del recurso.
            liberarRecursoAlKernel(instruccionActual.operando1);
        }
        else if (strcmp(instruccionActual.instruccion, "IO_GEN_SLEEP") == 0) {
            // Solicita sleep en interfaz de I/O.
            solicitarSleepIO(instruccionActual.operando1, atoi(instruccionActual.operando2));
        }
        else if (strcmp(instruccionActual.instruccion, "IO_STDIN_READ") == 0) {
            // Solicita lectura desde STDIN.
            leerDesdeStdin(instruccionActual.operando1, atoi(instruccionActual.operando2), atoi(instruccionActual.operando3));
        }
        else if (strcmp(instruccionActual.instruccion, "IO_STDOUT_WRITE") == 0) {
            // Solicita al Kernel imprimir por pantalla desde una dirección de memoria.
            escribirStdoutDesdeMemoria(instruccionActual.operando1, atoi(instruccionActual.operando2), atoi(instruccionActual.operando3));
        }
        else if (strcmp(instruccionActual.instruccion, "IO_FS_CREATE") == 0) {
            // Solicita al Kernel crear un archivo.
            crearArchivoEnFS(instruccionActual.operando1, instruccionActual.operando2);
        }
        else if (strcmp(instruccionActual.instruccion, "IO_FS_DELETE") == 0) {
            // Solicita al Kernel eliminar un archivo.
            eliminarArchivoEnFS(instruccionActual.operando1, instruccionActual.operando2);
        }
        else if (strcmp(instruccionActual.instruccion, "IO_FS_TRUNCATE") == 0) {
            // Solicita al Kernel modificar el tamaño de un archivo.
            truncarArchivoEnFS(instruccionActual.operando1, instruccionActual.operando2, atoi(instruccionActual.operando3));
        }
        else if (strcmp(instruccionActual.instruccion, "IO_FS_WRITE") == 0) {
            // Solicita al Kernel escribir en un archivo desde memoria.
            escribirArchivoDesdeMemoria(instruccionActual.operando1, instruccionActual.operando2, atoi(instruccionActual.operando3), atoi(instruccionActual.operando4), atoi(instruccionActual.operando5));
        }
        else if (strcmp(instruccionActual.instruccion, "IO_FS_READ") == 0) {
            // Solicita al Kernel leer de un archivo a memoria.
            leerArchivoAMemoria(instruccionActual.operando1, instruccionActual.operando2, atoi(instruccionActual.operando3), atoi(instruccionActual.operando4), atoi(instruccionActual.operando5));
        }
        else if (strcmp(instruccionActual.instruccion, "EXIT") == 0) {
            // Finaliza el proceso y devuelve el contexto de ejecución al Kernel.
            finalizarProcesoYDevolverContexto();
        }
        else {
            printf("Instrucción no reconocida: %s\n", instruccionActual.instruccion);
        }
}

void execute(t_instruccion instruccionActual) {
    if (strcmp(instruccionActual.instruccion, "MOV") == 0) {
        // Implementar lógica para MOV
    } else if (strcmp(instruccionActual.instruccion, "ADD") == 0) {
        // Lógica para ADD
    } else if (strcmp(instruccionActual.instruccion, "SUB") == 0) {
        // Lógica para SUB
    } else if (strcmp(instruccionActual.instruccion, "JNZ") == 0) {
        // Lógica para JNZ
    } else if (strcmp(instruccionActual.instruccion, "RESIZE") == 0) {
        // Lógica para RESIZE
    } else if (strcmp(instruccionActual.instruccion, "COPY_STRING") == 0) {
        // Lógica para COPY_STRING
    } else if (strcmp(instruccionActual.instruccion, "WAIT") == 0) {
        // Lógica para WAIT
    } else if (strcmp(instruccionActual.instruccion, "SIGNAL") == 0) {
        // Lógica para SIGNAL
    } else if (strcmp(instruccionActual.instruccion, "IO_GEN_SLEEP") == 0) {
        // Lógica para IO_GEN_SLEEP
    } else if (strcmp(instruccionActual.instruccion, "IO_STDIN_READ") == 0) {
        // Lógica para IO_STDIN_READ
    } else if (strcmp(instruccionActual.instruccion, "IO_STDOUT_WRITE") == 0) {
        // Lógica para IO_STDOUT_WRITE
    } else if (strcmp(instruccionActual.instruccion, "IO_FS_CREATE") == 0) {
        // Lógica para IO_FS_CREATE
    } else if (strcmp(instruccionActual.instruccion, "IO_FS_DELETE") == 0) {
        // Lógica para IO_FS_DELETE
    } else if (strcmp(instruccionActual.instruccion, "IO_FS_TRUNCATE") == 0) {
        // Lógica para IO_FS_TRUNCATE
    } else if (strcmp(instruccionActual.instruccion, "IO_FS_WRITE") == 0) {
        // Lógica para IO_FS_WRITE
    } else if (strcmp(instruccionActual.instruccion, "IO_FS_READ") == 0) {
        // Lógica para IO_FS_READ
    } else if (strcmp(instruccionActual.instruccion, "EXIT") == 0) {
        finalizarProcesoYDevolverContexto();
    } else {
        printf("Instrucción no reconocida: %s\n", instruccionActual.instruccion);
    }
}

void checkInterrupt(int pid, int conexionDispatch) {
    if (hayInterrupcionParaPID(pid)) {
        // Preparar el Contexto de Ejecución actualizado
       // ContextoEjecucion contexto = obtenerContextoActualizado();

        // Enviar el contexto al Kernel a través de la conexión de dispatch
        //enviarContexto(conexionDispatch, contexto);

        // Log para indicar que se ha manejado una interrupción
        printf("Interrupción manejada para el PID %d, contexto enviado al Kernel.\n", pid);

        // Detener la ejecución actual o preparar para una nueva instrucción según el diseño del sistema
        prepararParaNuevaInstruccion();
    } else {
        // No hay interrupción, continuar con la ejecución normal
        printf("No hay interrupción para el PID %d, continuando ejecución.\n", pid);
    }
}
*/
void recv_instruccion(int memoria_fd){

	t_list *paquete = recibir_paquete(memoria_fd);

	// Obtener el path del paquete
	char *instruccion_cadena = (char *)list_get(paquete, 0);

	log_info(loggerCpu, "Recibi la instruccion: %s", instruccion_cadena ); 

	char **palabras = string_split(instruccion_cadena , " ");
    
    //int tamanio_array = (sizeof(palabras) / sizeof(palabras[0]));

    int tamanio_array = 0;
    while ((palabras)[tamanio_array] != NULL) {
        tamanio_array++;
    }

	instruccionActual.instruccion = malloc(sizeof(char) * (strlen(palabras[1]) + 1));
    strcpy(instruccionActual.instruccion, palabras[0]); 

	instruccionActual.operando1 = malloc(sizeof(char) * (strlen(palabras[1]) + 1));
    strcpy(instruccionActual.operando1, palabras[1]); 

    if(tamanio_array > 2){

        instruccionActual.operando2 = malloc(sizeof(char) * (strlen(palabras[2]) + 1));
        strcpy(instruccionActual.operando2, palabras[2]);
    }

    if(tamanio_array > 3){

        instruccionActual.operando3 = malloc(sizeof(char) * (strlen(palabras[3]) + 1));
        strcpy(instruccionActual.operando3, palabras[3]);
    }

    if(tamanio_array > 4){

        instruccionActual.operando4 = malloc(sizeof(char) * (strlen(palabras[4]) + 1));
        strcpy(instruccionActual.operando4, palabras[4]);
    }

    if(tamanio_array > 5){

        instruccionActual.operando5 = malloc(sizeof(char) * (strlen(palabras[5]) + 1));
        strcpy(instruccionActual.operando5, palabras[5]);
    }
	

        /*

    // Liberar memoria asignada a palabras
    int i = 0;
    while (palabras[i] != NULL) {
        free(palabras[i]);
        i++;
    } */
    free(palabras);
	free(paquete);
    
}