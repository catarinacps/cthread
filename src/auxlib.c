#include "../include/auxlib.h"
#include "../include/support.h"

int lastTid = TID_MAIN;	// o tid inicia em zero
int tidExec = TID_MAIN; // pelo menos no inicio

void dispatcher(int terminado) {
    TCB_t *proxThread, *retThread, *bloqThread;
    int *ptTidBloq;
    LISTA *nodo;
    ESPERA *aux;

    if (terminado == 1) {				
        retThread = findTCB(tidExec);
        retThread->state = 4; // estado term

        if (!emptyLista(esperando)) {								///////////////////////////
            nodo = esperando;										// CJOIN PART2
            aux = (ESPERA *)nodo->dados;							// 
																	// tem alguem bloqueado esperando
            while (aux->tidEsperado != tidExec && nodo != NULL) {	// entao:
                nodo = getNextNodeLista(esperando);					//  procurar se essa thread que acabou
                if (nodo != NULL) {									//  de terminar estava sendo esperada por outra
                    aux = (ESPERA *)nodo->dados;					//
                }													///////////////////////////
            }

            if (aux->tidEsperado == tidExec) {								///////////////////////////
                bloqThread = findTCB(aux->tidBloqueado);					//
                ptTidBloq = malloc(sizeof(int));							// ela estava sendo esperada por outra...
                *ptTidBloq = bloqThread->tid;								//
                bloqThread->state = 1; // estado apto						// passar a outra pra apto e
                AppendFila2(aptos[bloqThread->ticket], (void *)ptTidBloq);	// remove-la da lista de bloqueados
                esperando = removeLista(nodo, 0);							///////////////////////////
            }
        }
    }

    if (!emptyAptos()) {			
        tidExec = nextApto();  /// UNICA INSTRUÇAO QUE MODIFICA tidExec		
        proxThread = findTCB(tidExec);										/////////////////////////
																			//
        proxThread->state = 2; // estado exec								//	ESCALONAMENTO
																			//	PADRÃO
        setcontext(&(proxThread->context));									//
    } else {																/////////////////////////
        printf("ALGO MUITO ERRADO ESTÁ ACONTECENDO");		
    }
}
// 0: Criacao; 1: Apto; 2: Execucao; 3: Bloqueado e 4: Termino
///-----------------------------------------------------------------------------------------------------------------
int getNextTid() {
    return lastTid++;
}
///-----------------------------------------------------------------------------------------------------------------
// inicializa listas e filas globais
void initializeLib() {		
    int i;

    threads = initLista();
    esperando = initLista();

    for (i = 0; i < 4; i++) {
        aptos[i] = malloc(sizeof(PFILA2));
    }
    
    CreateFila2(aptos[0]);
    CreateFila2(aptos[1]);
    CreateFila2(aptos[2]);
    CreateFila2(aptos[3]);
}
///-----------------------------------------------------------------------------------------------------------------
// diz se a lista de threads está vazia
int emptyTCBList() {
    return emptyLista(threads);
}
///-----------------------------------------------------------------------------------------------------------------
// adiciona um TCB a lista de threads
void addTCB(TCB_t *tcb) {
    threads = insertLista(threads, (void *)tcb);
}
///-----------------------------------------------------------------------------------------------------------------
// retorna o endereço do TCB com a tid se encontrado, senao retorna NULL
TCB_t *findTCB(int tid) {
    return (TCB_t *)getNodeLista(threads, tid);
}
///-----------------------------------------------------------------------------------------------------------------
// troca de fila de prioridades
void changePrioFIFO(int prio_velha, int prio_nova, int tid) {
    int *dado_tid, achou = 0;

    FirstFila2(aptos[prio_velha]); // coloca iterador no inicio da fila

    do {
        dado_tid = (int *)GetAtIteratorFila2(aptos[prio_velha]);
        if (*dado_tid == tid) {
            achou = 1;
        } else {
            NextFila2(aptos[prio_velha]);
        }
    } while (achou == 0); // coloca iterador na posicao com tid

    DeleteAtIteratorFila2(aptos[prio_velha]); // remove da fila

    AppendFila2(aptos[prio_nova], (void *)dado_tid); // adiciona na outra fila
}
///-----------------------------------------------------------------------------------------------------------------
// retorna 1 se nao ha aptos, senao retorna 0
int emptyAptos() {
    if ((FirstFila2(aptos[0]) != 0) && (FirstFila2(aptos[1]) != 0) &&
        (FirstFila2(aptos[2]) != 0) && (FirstFila2(aptos[3]) != 0)) {
        return 1;
    } else {
        return 0;
    }
}
///-----------------------------------------------------------------------------------------------------------------
// remove apto de maior prioridade da fila e retorna seu tid
int nextApto() {
    int tid, prio;
    // pelo projeto do dispatcher, garante-se q ha aptos
    if (FirstFila2(aptos[0]) == 0) {
        prio = 0;
    } else if (FirstFila2(aptos[1]) == 0) {
        prio = 1;
    } else if (FirstFila2(aptos[2]) == 0) {
        prio = 2;
    } else {
        FirstFila2(aptos[3]);
        prio = 3;
    }
    tid = *((int *)GetAtIteratorFila2(aptos[prio]));
    DeleteAtIteratorFila2(aptos[prio]);

    return tid;
}
