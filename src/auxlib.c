#include "../include/support.h"
#include "../include/cdata.h"
#include "lista.c"

#include <ucontext.h>

LISTA *threads, *esperando, *semaforos;

PFILA2 aptos[4];

/*

*/
void dispatcher() {
    int tidProx, tidRet;
    TCB_t *proxThread, *retThread;

    if (!emptyAptos()) {
        tidProx = nextApto();
        proxThread = findTCB(tidProx);

        proxThread->state = 2;

        if ((tidRet = findNodeExec(tidProx)) != -1) {
            retThread = findTCB(tidRet);
            retThread->state = 4;
        }

        setcontext(&(proxThread->context));
    } else {
        printf("PODE SEPA SER EXECUTADA?VER COM CECHIN FALANDO NISSO EAE "
               "CECHIN BELEZA TUDO BEM COMO FOI O DIA DESTE BELO PROFESSOR "
               "ENSINA MUITO BEM 10/10 <3");
    }
}
// 0: Criacao; 1: Apto; 2: Execucao; 3: Bloqueado e 4: Termino


int findNodeExec(int tidIgnore) {
    LISTA *auxPt;
    TCB_t *auxTCB;

    auxPt = getFirstNodeLista(threads);
    auxTCB = (TCB_t *)auxPt->dado;
    while (auxTCB->state != 2 && auxPt != NULL && auxTCB->tid != tidIgnore) {
        auxPt = getNextNodeLista(threads);
        auxTCB = (TCB_t *)auxPt->dado;
    }

    if (auxPt == NULL) {
        return -1;
    } else {
        return auxTCB->tid;
    }
}

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
    if (threads == NULL) {
        return 1;
    } else {
        return 0;
    }
}

// adiciona um TCB a lista de threads
void addTCB(TCB_t *tcb) {
    insertLista(threads, (*void)tcb);
}

// retorna o endereço do TCB com a tid se encontrado, senao retorna NULL
TCB_t *findTCB(int tid) {
    return (*TCB_t)getNodeLista(threads, tid);
}

// troca de fila de prioridades
void changePrioFIFO(int prio_velha, int prio_nova, int tid) {
    int *dado_tid, achou = 0;

    FirstFila2(aptos[prio_velha]); // coloca iterador no inicio da fila
    dado_tid = malloc(sizeof(int));

    do {
        dado_tid = GetAtIteratorFila2(aptos[prio_velha]);
        if (*dado_tid = td) {
            achou = 1;
        } else {
            NextFila2(aptos[prio_velha]);
        }
    } while (achou == 0); // coloca iterador na posicao com tid

    DeleteAtIteratorFila2(aptos[prio_velha]); // remove da lista

    AppendFila2(aptos[prio_nova], (*void)dado_tid); // adiciona na outra lista
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
	tid=*((*int)GetAtIteratorFila2(aptos[prio]));
	DeleteAtIteraatorFila2(aptos[prio]);

	return tid;
}
