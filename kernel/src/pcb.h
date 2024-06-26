#ifndef PCB_H_
#define PCB_H_

#include <utils.h>
#include <pthread.h>
#include <semaphore.h>

typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
} Estado;

typedef struct {
    uint32_t PC;
    uint8_t AX;
    uint8_t BX;
    uint8_t CX;
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t SI;
    uint32_t DI;
} CPU_Registers;

typedef struct {
    int PID; //id del proceso
    int pc; //direccionamiento
    int quantum;
    Estado estado; // duración del quantum 
    CPU_Registers* cpuRegisters; // puntero a cantidad de registros de la cpu (el valor lo tendría la cpu)
    char* path;
} PCB;

extern int pid_counter;
extern pthread_mutex_t mutex_pid;

int generarPID(void);
PCB* crearPCB(char* path);


#endif /* PCB_H_ */

