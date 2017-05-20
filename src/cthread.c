#include "../include/auxlib.h"
#include <string.h>

int started = 0;

int ccreate(void *(*start)(void *), void *arg, int prio) {
    TCB_t *tcbMain, *tcbNew;
    int *ptTidNew;
    ucontext_t *contextoDisp;

    if (prio < 0 || prio > 4) { // se a prioridade não é valida dá erro
        return ERRO;
    } else if (started == 0) { // inicializa a lib na primeira vez
        initializeLib();
        started = 1;
    } 
	
	if (emptyTCBList()) {	// se a lista de threads está vazia
							// precisamos criar a tcb da main
        tcbMain = malloc(sizeof(TCB_t));

        tcbMain->tid = getNextTid();
        tcbMain->state = 2;		// estado exec
        tcbMain->ticket = 0;
        getcontext(&(tcbMain->context));

        tidExec = tcbMain->tid;

        contextoDisp = malloc(sizeof(ucontext_t));						///////////////////////
        getcontext(contextoDisp);
        contextoDisp->uc_stack.ss_sp = (char *)malloc(SIGSTKSZ);
        contextoDisp->uc_stack.ss_size = SIGSTKSZ;						// ajusta contexto do dispatcher
        contextoDisp->uc_link = NULL;
        makecontext(contextoDisp, (void (*)(void))dispatcher, 1, 1);	///////////////////////

        tcbMain->context.uc_stack.ss_sp = (char *)malloc(SIGSTKSZ);
        tcbMain->context.uc_stack.ss_size = SIGSTKSZ;
        tcbMain->context.uc_link = contextoDisp;	// após a main, chamar dispatcher

        addTCB(tcbMain);
    }

	// criar a nova tcb e adicionar à lista de threads
	
    tcbNew = malloc(sizeof(TCB_t));

    getcontext(&(tcbNew->context));
    tcbNew->tid = getNextTid();
    tcbNew->state = 1;	// estado apto
    tcbNew->ticket = prio;

    contextoDisp = malloc(sizeof(ucontext_t));					////////////////////
    getcontext(contextoDisp);
    contextoDisp->uc_stack.ss_sp = (char *)malloc(SIGSTKSZ);	// ajusta contexto do dispatcher
    contextoDisp->uc_stack.ss_size = SIGSTKSZ;
    contextoDisp->uc_link = NULL;
    makecontext(contextoDisp, (void (*)(void))dispatcher, 1, 1);///////////////////

    tcbNew->context.uc_stack.ss_sp = (char *)malloc(SIGSTKSZ);
    tcbNew->context.uc_stack.ss_size = SIGSTKSZ;
    tcbNew->context.uc_link = contextoDisp;						// quando a thread terminar, chamar dispatcher
    makecontext(&(tcbNew->context), (void (*)(void))start, 1, arg);

    addTCB(tcbNew);

    ptTidNew = malloc(sizeof(int));
    *ptTidNew = tcbNew->tid;
    AppendFila2(aptos[prio], (void *)ptTidNew);		// insere a nova thread na fila de aptos

    return tcbNew->tid;
}
///-----------------------------------------------------------------------------------------------------------------
int csetprio(int tid, int prio) {
    TCB_t *tcbAux;

    if (prio < 0 || prio > 4) {     // se a prioridade não é valida dá erro
        return ERRO;
    }
	
    if ((tcbAux = findTCB(tid))) {
        if (tcbAux->state == 1) {  // estado apto
            changePrioFIFO(tcbAux->ticket, prio, tid);
        }
        tcbAux->ticket = prio;

        return OK;
    } else {
        return ERRO;
    }
}
///-----------------------------------------------------------------------------------------------------------------
int cyield(void) {
    int *ptTidExec;
    TCB_t *tcbExec;

    if ((emptyTCBList())) {
        return ERRO;
    } else {
        ptTidExec = malloc(sizeof(int));
        *ptTidExec = tidExec;

        tcbExec = findTCB(tidExec);
		
        if (AppendFila2(aptos[tcbExec->ticket], (void *)ptTidExec) == 0) { //pega a thread executando no momento
            tcbExec->state = 1;	// estado apto								 e passa ela para apto
            getcontext(&(tcbExec->context));
        } else {
            return ERRO;
        }

        if (tcbExec->state == 1) { // estado apto...time loop enemy
            dispatcher(-1);
        }
        return OK;
    }
}
///-----------------------------------------------------------------------------------------------------------------
int cjoin(int tid) {
    TCB_t *tcbAux;
    LISTA *nodo;
    ESPERA *aux;

    nodo = esperando;

    if (!(tcbAux = findTCB(tid))) {	//  a thread a ser esperada nao consta na lista de threads
        return ERRO;
    }
    if (tcbAux->state == 4) { //  a thread a ser esperada ja foi terminada
        return ERRO;
    }
	
    if (emptyLista(esperando)) { 
        tcbAux = findTCB(tidExec);
        tcbAux->state = 3;	// estado bloq

        aux = malloc(sizeof(ESPERA));					//////////////////////////////////////////////////////////////
        aux->tidEsperado = tid;							// se nao tem nenhuma thread esperando outra, só precisamos
        aux->tidBloqueado = tidExec;					// bloquear a thread atual e inserir ela
        esperando = insertLista(esperando, (void *)aux);// na lista de espera
														////////////////////////////////////////////////////////////
        getcontext(&(tcbAux->context));

    } else {
        do {												//////////////////////////////////////////////////////////////
            aux = (ESPERA *)nodo->dados;					// se ja tem threads na lista, entao precisamos checar se alguma
            nodo = getNextNodeLista(esperando);				// delas é a mesma do parametro (ou seja, ja esta sendo esperada)
        } while (aux->tidEsperado != tid && nodo != NULL);	//////////////////////////////////////////////////////////////

        if (nodo == NULL && aux->tidEsperado != tid) {
            tcbAux = findTCB(tidExec);							//////////////////////////////////////////////////////////////
            tcbAux->state = 3; // estado bloq

            aux = malloc(sizeof(ESPERA));						// a thread nao está sendo esperada ainda, entao só
            aux->tidEsperado = tid;								// precisamos bloquear a thread atual e inserir ela
            aux->tidBloqueado = tidExec;						// na lista de espera
            esperando = insertLista(esperando, (void *)aux);	//////////////////////////////////////////////////////////////

            getcontext(&(tcbAux->context));
			
        } else {
            return ERRO; 	// thread a ser esperada ja esta sendo esperada por outra
        } 
    }
    if (tcbAux->state == 3) { // estado bloq...time loop enemy
        dispatcher(-1);
    }
    return OK;
}
///-----------------------------------------------------------------------------------------------------------------
int csem_init(csem_t *sem, int count) {
    if (started == 0) { // inicializa a lib na primeira vez
        initializeLib();
        started = 1;
    }

    if (count >= 0) {	// inicializa o recurso e a fila de bloqueados do semaforo
        sem->count = count;
        sem->fila = malloc(sizeof(PFILA2));
		
        if (CreateFila2(sem->fila) != 0) {
            return ERRO;
        } else {
            return OK;
        }
    } else {
        return ERRO;
    }
}
///-----------------------------------------------------------------------------------------------------------------
int cwait(csem_t *sem) {
    TCB_t *tcbAux;
    int *tidBloqueado;

    if (sem == NULL) {
        return ERRO;
    } else {
        if (sem->count > 0) { // se há recursos disponiveis no semáforo
            sem->count--;	  // vamos utiliza-lo

            return OK;
        } else {
            tcbAux = findTCB(tidExec);								///////////////////////////////////////////////
            tcbAux->state = 3; // estado bloq

            tidBloqueado = malloc(sizeof(int));						// se NÃO há recursos disponiveis
            *tidBloqueado = tidExec;								// precisamos bloquear a thread atual

            if (AppendFila2(sem->fila, (void *)tidBloqueado) != 0) {// e inseri-la na lista de bloqueados
                return ERRO;										// do semáforo
            }														///////////////////////////////////////////////

            getcontext(&(tcbAux->context));

            if (tcbAux->state == 3) { // estado bloq...time loop enemy
                dispatcher(-1);
            }
            return OK;
        }
    }
}
///-----------------------------------------------------------------------------------------------------------------
int csignal(csem_t *sem) {
    int tidBloqueado, *tidApto;
    TCB_t *tcbApto;

    if (sem == NULL) {
        return ERRO;
    } else {
        if (FirstFila2(sem->fila) != 0) {	// nao há nenhuma thread bloqueada nesse semaforo, então ja ta pronto
			sem->count++;		// desaloca o recurso
            return OK;
        } else {
            tidBloqueado = *((int *)GetAtIteratorFila2(sem->fila));			//////////////////////////////
            DeleteAtIteratorFila2(sem->fila);								// 
																			// pega a primeira thread bloqueada
            tcbApto = findTCB(tidBloqueado);								// da fila, coloca ela em apto
            tidApto = malloc(sizeof(int));									// e remove da fila de bloqueados
            *tidApto = tidBloqueado;										//
																			//
            if (AppendFila2(aptos[tcbApto->ticket], (void *)tidApto) == 0) {//////////////////////////////
                tcbApto->state = 1; // estado apto
                return OK;
            } else {
                return ERRO;
            }
        }
    }
}
///-----------------------------------------------------------------------------------------------------------------
int cidentify(char *name, int size) {
    char nomes[120];

    if (started == 0) { // inicializa a lib na primeira vez
        initializeLib();
        started = 1;
    }

    strcpy(nomes, "Gabriel Stefaniak Niemiec 262503\n"
                  "Henrique Correa Pereira da Silva 262508\n"
                  "Nicolas Eymael da Silva 262506\n");
    if (strlen(nomes) <= size) {
        strcpy(name, nomes);
        return OK;
    } else {
        return ERRO;
    }
}
