#include "../include/auxlib.h"
#include <string.h>

int started=0;
//ucontext_t *contexto;


int ccreate(void *(*start)(void *), void *arg, int prio) {
    TCB_t *tcbMain, *tcbNew;
	int *ptTidNew;
	ucontext_t *contexto;
	
    if (prio < 0 || prio > 4) { //se a prioridade n eh valida da erro
        return -1;
    }else if(started==0){	//inicializa a lib n primeira vez
	initializeLib();
	started=1;
	//cria tcb da main
        tcbMain = malloc(sizeof(TCB_t));

        tcbMain->tid = getNextTid();
        tcbMain->state = 2;
        tcbMain->ticket = 0;
        if (getcontext(&(tcbMain->context)) != 0) {
            return -1;
        }
		
        tidExec=tcbMain->tid;
		
	contexto=malloc(sizeof(ucontext_t));
	getcontext(contexto);
	contexto->uc_stack.ss_sp = (char *) malloc(SIGSTKSZ);
        contexto->uc_stack.ss_size = SIGSTKSZ;
        contexto->uc_link = NULL;
	makecontext(contexto, (void (*)(void))dispatcher, 1,0);
		
        tcbMain->context.uc_stack.ss_sp = (char *) malloc(SIGSTKSZ);
        tcbMain->context.uc_stack.ss_size = SIGSTKSZ;
	tcbMain->context.uc_link=contexto;

        addTCB(tcbMain);
    }

    tcbNew = malloc(sizeof(TCB_t));

    getcontext(&(tcbNew->context));
    tcbNew->tid = getNextTid();
    tcbNew->state = 1;
    tcbNew->ticket = prio;
	
	//fazer coisas deselegantes
	contexto=malloc(sizeof(ucontext_t));
	getcontext(contexto);
	contexto->uc_stack.ss_sp = (char *) malloc(SIGSTKSZ);
    contexto->uc_stack.ss_size = SIGSTKSZ;
    contexto->uc_link = NULL;
	makecontext(contexto, (void (*)(void))dispatcher, 1,0);

    tcbNew->context.uc_stack.ss_sp = (char *) malloc(SIGSTKSZ);
    tcbNew->context.uc_stack.ss_size = SIGSTKSZ;
	tcbNew->context.uc_link=contexto;
    makecontext(&(tcbNew->context), (void (*)(void))start, 1, arg);
	
	//tcbNew->context.uc_link=contexto;
    addTCB(tcbNew);
	
	ptTidNew=malloc(sizeof(int));
	*ptTidNew=tcbNew->tid;
	AppendFila2(aptos[prio], (void *)ptTidNew);

    return tcbNew->tid;
}

int csetprio(int tid, int prio) {
    TCB_t *tcbAux;

    if (prio < 0 || prio > 4) {
        return -1;
    }
    if ((tcbAux = findTCB(tid))) {
        if (tcbAux->state == 1) {
            changePrioFIFO(tcbAux->ticket, prio, tid);
        }
        tcbAux->ticket = prio;

        return 1;
    } else {
        return -1;
    }
}

int cyield(void) {
    int *ptTidExec;
    TCB_t *tcbExec;

    if ((emptyTCBList())) {
        return -1;
    } else {
        ptTidExec = malloc(sizeof(int));
        *ptTidExec = tidExec;

        tcbExec = findTCB(tidExec);
        if (AppendFila2(aptos[tcbExec->ticket], (void *)ptTidExec) == 0) {
            tcbExec->state = 1;
            getcontext(&(tcbExec->context));
        } else {
            return -1;
        }

        if (tcbExec->state == 1) {
            dispatcher(-1);
        }
        return 1;
    }
}

int cjoin(int tid) {
    TCB_t *tcbAux;
    LISTA *nodo;
    ESPERA *aux;

    nodo = esperando;
	
    if (!(tcbAux = findTCB(tid))) {
        return -1;
    }
    if (tcbAux->state == 4) {
        return -1;
    }
    if (emptyLista(esperando)) {
        tcbAux = findTCB(tidExec);
        tcbAux->state = 3;

        aux = malloc(sizeof(ESPERA));
        aux->tidEsperado = tid;
        aux->tidBloqueado = tidExec;
        esperando = insertLista(esperando, (void *)aux);

        getcontext(&(tcbAux->context));

    } else {
        do {
            aux = (ESPERA *)nodo->dados;
            nodo = getNextNodeLista(esperando);
        } while (aux->tidEsperado != tid && nodo != NULL);

        if (nodo == NULL && aux->tidEsperado != tid) {
            tcbAux = findTCB(tidExec);
            tcbAux->state = 3;

            aux = malloc(sizeof(ESPERA));
            aux->tidEsperado = tid;
            aux->tidBloqueado = tidExec;
            esperando = insertLista(esperando, (void *)aux);

            getcontext(&(tcbAux->context));
        } else {
            return -1;	//thread a ser esperada ja esta
        }				//sendo esperada por outra
    }
    if (tcbAux->state == 3) {
        dispatcher(-1);
    }
    return 0;
}

int csem_init(csem_t *sem, int count) {
    
    if (count >= 0) {
        sem->count = count;
	sem->fila = malloc(sizeof(PFILA2));
        // add lista de semaforos da auxlib?
        if (CreateFila2(sem->fila) != 0) {
            return -1;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}

int cwait(csem_t *sem) {
    TCB_t *tcbAux;
    int *tidBloqueado;

    if (sem == NULL) {
        return -1;
    } else {
        if (sem->count > 0) {
            sem->count--;

            return 0;
        } else {
            tcbAux = findTCB(tidExec);
            tcbAux->state = 3;

            tidBloqueado = malloc(sizeof(int));
            *tidBloqueado = tidExec;

            if (AppendFila2(sem->fila, (void *)tidBloqueado) != 0) {
                return -1;
            }

            getcontext(&(tcbAux->context));

            if (tcbAux->state == 3) {
                dispatcher(-1);
            }
            return 0;
        }
    }
}

int csignal(csem_t *sem) {
    int tidBloqueado, *tidApto;
    TCB_t *tcbApto;

    if (sem == NULL) {
        return -1;
    } else {
        sem->count++;
        if (FirstFila2(sem->fila) != 0) {
            return 0;
        } else {
            tidBloqueado = *((int *)GetAtIteratorFila2(sem->fila));
            DeleteAtIteratorFila2(sem->fila);

            tcbApto = findTCB(tidBloqueado);
            tidApto = malloc(sizeof(int));
            *tidApto = tidBloqueado;

            if (AppendFila2(aptos[tcbApto->ticket], (void *)tidApto) == 0) {
                tcbApto->state = 1;
                return 0;
            } else {
                return -1;
            }
        }
    }
}

int cidentify(char *name, int size) {
    char nomes[120];
    strcpy(nomes, "Gabriel Stefaniak Niemiec 262503\n"
                  "Henrique Correa Pereira da Silva 262508\n"
                  "Nicolas Eymael da Silva 262506\n");
    if (strlen(nomes) <= size) {
        strcpy(name, nomes);
        return 0;
    } else {
        return -1;
    }
}
