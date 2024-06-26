#include <utils.h>
#include "../../kernel/src/kernel.h"
#include "/home/utnso/tp-2024-1c-File-System-Fanatics/kernel/src/pcb.h"

void fetch();

void decode_execute();

void checkInterrupt();

void recibir_proceso(int socket_fd);

void modificarPCB(PCB* proceso, int new_pid, int new_pc);



