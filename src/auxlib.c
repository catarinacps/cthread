#include "../include/support.h"
#include "../include/cdata.h"
#include "lista.c"

#include <ucontext.h>

LISTA *threads, *esperando, *semaforos;

PFILA2 aptos[4];

// inicializa listas e filas
void initializeLib() {
    threads = initLista();
    esperando = threads;
    semaforos = threads;
    CreateFila2(aptos[0);
    CreateFila2(aptos[1]);
    CreateFila2(aptos[2]);
    CreateFila2(aptos[3]);
}

// diz se a lista de threads está vazia
int Threads_vazia() {
    if (threads == NULL) {
        return 1;
    } else {
        return 0;
    }
}

// adiciona um TCB a lista de threads
void Add_TCB(TCB_t *tcb) {
    insertLista(threads, (*void)tcb);
}

// retorna o endereço do TCB com a tid se encontrado, senao retorna NULL
TCB_t *Acha_TID_threads(int tid) {
    return (*TCB_t)getNodeLista(threads, tid);
}

// troca de fila de prioridades
void Troca_prioridade(int prio_velha, int prio_nova, int tid) {
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
int Sem_aptos() {
    if ((FirstFila2(aptos[0]) != 0) && (FirstFila2(aptos[1]) != 0) &&
        (FirstFila2(aptos[2]) != 0) && (FirstFila2(aptos[3]) != 0)) {
        return 1;
    } else {
        return 0;
    }
}

// remove apto de maior prioridade e retorna tid, senao retorna -1
int Proximo_apto() {
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
