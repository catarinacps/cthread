#include "../include/auxlib.h"

void dispatcher(int tidTerm) {
    TCB_t *proxThread, *retThread;

    if (!emptyAptos()) {
        tidExec = nextApto();
        proxThread = findTCB(tidExec);

        proxThread->state = 2;

        if (tidTerm >= 0) {
            retThread = findTCB(tidTerm);
            retThread->state = 4;
        }

        setcontext(&(proxThread->context));
    } else {
        printf("PODE SEPA SER EXECUTADA? VER COM CECHIN FALANDO NISSO EAE "
               "CECHIN BELEZA TUDO BEM COMO FOI O DIA DESTE BELO PROFESSOR "
               "ENSINA MUITO BEM 10/10 <3");
    }
}
// 0: Criacao; 1: Apto; 2: Execucao; 3: Bloqueado e 4: Termino

int getNextTid() {
    return lastTid++;
}

void setTidExec(int tid) {
    tidExec = tid;
}

/* acho q n vamos mais usar essa merda!!!!
TCB_t * findNodeExec() {
    LISTA *auxPt;
    TCB_t *auxTCB;

    auxPt = getFirstNodeLista(threads);
    auxTCB = (TCB_t *)auxPt->dado;
    while (auxTCB->state != 2 && auxPt != NULL) {
        auxPt = getNextNodeLista(threads);
        auxTCB = (TCB_t *)auxPt->dado;
    }

    if (auxPt == NULL) {
        return -1;
    } else {
        return auxTCB;
    }
}
*/

// inicializa listas e filas
void initializeLib() {
    threads = initLista();
    esperando = threads;
    semaforos = threads;
    CreateFila2(aptos[0]);
    CreateFila2(aptos[1]);
    CreateFila2(aptos[2]);
    CreateFila2(aptos[3]);
}

// diz se a lista de threads está vazia
int emptyTCBList() {
    return emptyLista(threads);
}

// adiciona um TCB a lista de threads
void addTCB(TCB_t *tcb) {
    threads=insertLista(threads, (void*)tcb);
}

// retorna o endereço do TCB com a tid se encontrado, senao retorna NULL
TCB_t *findTCB(int tid) {
    return (TCB_t*)getNodeLista(threads, tid);
}

// troca de fila de prioridades
void changePrioFIFO(int prio_velha, int prio_nova, int tid) {
    int *dado_tid, achou = 0;

    FirstFila2(aptos[prio_velha]); // coloca iterador no inicio da fila

    do {
        dado_tid = (int*)GetAtIteratorFila2(aptos[prio_velha]);
        if (*dado_tid == tid) {
            achou = 1;
        } else {
            NextFila2(aptos[prio_velha]);
        }
    } while (achou == 0); // coloca iterador na posicao com tid

    DeleteAtIteratorFila2(aptos[prio_velha]); // remove da fila

    AppendFila2(aptos[prio_nova], (void*)dado_tid); // adiciona na outra fila
}

// retorna 1 se nao ha aptos, senao retorna 0
int emptyAptos() {
    if ((FirstFila2(aptos[0]) != 0) && (FirstFila2(aptos[1]) != 0) &&
        (FirstFila2(aptos[2]) != 0) && (FirstFila2(aptos[3]) != 0)) {
        return 1;
    } else {
        return 0;
    }
}

// remove apto de maior prioridade e retorna tid
int nextApto() {
    int tid, prio;
    // pelo projeto do dispatcher, garante-se q ha aptos
    if (FirstFila2(aptos[0]==0) {
        prio = 0;
	} else if (FirstFila2(aptos[1])==0) {
        prio = 1;
	} else if (FirstFila2(aptos[2])==0) {
        prio = 2
	} else {
        FirstFila2(aptos[3]);
        prio = 3;
	}
	tid=*((int*)GetAtIteratorFila2(aptos[prio]));
	DeleteAtIteraatorFila2(aptos[prio]);

	return tid;
}
