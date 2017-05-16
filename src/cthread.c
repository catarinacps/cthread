#include "../include/cthread.h"
#include "../include/auxlib.h"
#include <string.h>

int ccreate(void *(*start)(void *), void *arg, int prio) {
    TCB_t *tcbMain, *tcbNew;
    char mainStack[SIGSTKSZ], newStack[SIGSTKSZ];

    if (prio < 0 || prio > 4) {
        return -1;
    }
    if (emptyTCBList()) {
        tcbMain = malloc(sizeof(TCB_t));

        tcbMain->tid = getNextTid();
        tcbMain->state = 2;
        tcbMain->ticket = 0;
        if (getcontext(&(tcbMain->context)) != 0) {
            return -1;
        }

        setTidExec(tcbMain->tid);

        makecontext(tcbMain->context.uc_link, (void (*)(void))dispatcher, 1,
                    tcbMain->tid);
        tcbMain->context.uc_stack.ss_sp = mainStack;
        tcbMain->context.uc_stack.ss_size = sizeof(mainStack);

        addTCB(tcbMain);
    }

    tcbNew = malloc(sizeof(TCB_t));

    tcbNew->tid = getNextTid();
    tcbNew->state = 1;
    tcbNew->ticket = prio;
    makecontext(&(tcbNew->context), (void (*)(void))start, 1, arg);

    makecontext(tcbNew->context.uc_link, (void (*)(void))dispatcher, 1,
                tcbNew->tid);
    tcbNew->context.uc_stack.ss_sp = newStack;
    tcbNew->context.uc_stack.ss_size = sizeof(newStack);

    addTCB(tcbNew);

    return tcbNew->tid;
}

int csetprio(int tid, int prio) {
    TCB_t *tcbAux;

    if (prio < 0 || prio > 4) {
        return -1;
    }
    if (tcbAux = findTCB(tid)) {
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

    if (emptyTCB()) {
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

    if (tcbAux = findTCB(tid)) {
        return -1;
    }
    if (tcbAux->state == 4) {
        return -1;
    }

    if (emptylista(esperando)) {
        // so botar
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

        if (nodo == NULL) {
            // so botar
            tcbAux = findTCB(tidExec);
            tcbAux->state = 3;

            aux = malloc(sizeof(ESPERA));
            aux->tidEsperado = tid;
            aux->tidBloqueado = tidExec;
            esperando = insertLista(esperando, (void *)aux);

            getcontext(&(tcbAux->context));
        } else {
            return -1;
        }
    }
    if (tcbAux->state == 3) {
        dispatcher(-1);
    }
    return 0;
}

int csem_init(csem_t *sem, int count) {
    *sem->count = count;
    // add lista de semaforos da auxlib?
    if (CreateFila2(*sem->fila) != 0) {
        return -1;
    } else {
        return 0;
    }
}

int cwait(csem_t *sem) {
    TCB_t tcbAux;
    int *tidBloqueado;

    if (sem == NULL) {
        return -1;
    }

    if (*sem->count > 0) {
        *sem->count--;

        return 0;
    } else {
        tcbAux = findTCB(tidExec);
        tcbAux->state = 3;

        tidBloqueado = malloc(sizeof(int));
        *tidBloqueado = tidExec;

        if (AppendFila2(*sem->fila, (void *)tidBloqueado) != 0) {
            return -1;
        }

        getcontext(&(tcbAux->context));

        if (tcbAux->state == 3) {
            dispatcher(-1);
        }
        return 0;
    }
}

int csignal(csem_t *sem) {
    int tidBloqueado, *tidApto;
    TCB_t *tcbApto;

    if (sem == NULL) {
        return -1;
    }
    *sem->count++;
    if (FirstFila2(*sem->fila) != 0) {
        return 0;
    } else {
        tidBloqueado = *((int *)GetAtIteratorFila2(*sem->fila));
        DeleteAtIteratorFila2(*sem->fila);

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
