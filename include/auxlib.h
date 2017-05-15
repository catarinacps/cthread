#include "cdata.h"
#include "lista.h"

#include <stdio.h>
#include <ucontext.h>

#define TID_MAIN 0

LISTA *threads, *esperando, *semaforos;
PFILA2 aptos[4];

struct esperanca{
	int tidEsperado;
	int tidBloqueado;	//botar TCB*?
};
typedef struct esperanca ESPERA;

int lastTid = TID_MAIN;
int tidExec = TID_MAIN; //pelo menos no inicio


void dispatcher(int tidTerm);

int getNextTid();

void setTidExec(int tid);

void initializeLib();

int emptyTCBList();

void addTCB(TCB_t *tcb);

TCB_t *findTCB(int tid);

void changePrioFIFO(int prio_velha, int prio_nova, int tid);

int emptyAptos();

int nextApto();