#include "../include/cthread.h"
#include "../include/auxlib.h"


int ccreate (void* (*start)(void*), void *arg, int prio) {
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

        makecontext(tcbMain->context.uc_link, (void (*)(void)) dispatcher, 1, tcbMain->tid);
        tcbMain->context.uc_stack.ss_sp = mainStack;
        tcbMain->context.uc_stack.ss_size = sizeof(mainStack);

        addTCB(tcbMain);
    }

    tcbNew = malloc(sizeof(TCB_t));

    tcbNew->tid = getNextTid();
    tcbNew->state = 1;
    tcbNew->ticket = prio;
    makecontext(&(tcbNew->context), (void (*)(void)) start, 1, arg);

    makecontext(tcbNew->context.uc_link, (void (*)(void)) dispatcher, 1, tcbNew->tid);
    tcbNew->context.uc_stack.ss_sp = newStack;
    tcbNew->context.uc_stack.ss_size = sizeof(newStack);

    addTCB(tcbNew);

    return tcbNew->tid;
}

int csetprio (int tid, int prio) {
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

int cyield (void) {
    int *ptTidExec;
    TCB_t *tcbExec;

    if (emptyTCB()) {
        return -1;
    } else {
		ptTidExec=malloc(sizeof(int));
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

int cjoin (int tid) {
    TCB_t *tcbAux;
	LISTA *nodo;
	ESPERA *aux;
	
	nodo=esperando;
	
	if(tcbAux=findTCB(tid)){
		return -1;
	}
	if(tcbAux->state==4){
		return -1;
	}
	
	if(emptylista(esperando)){
		//so botar
		tcbAux=findTCB(tidExec);
		tcbAux->state=3;
		
		aux=malloc(sizeof(ESPERA));
		aux->tidEsperado=tid;
		aux->tidBloqueado=tidExec;
		esperando=insertLista(esperando,(void*)aux);
		
		getcontext(&(tcbAux->context));
		
		
	}else{
		
		do{
			aux = (ESPERA *)nodo->dados;
			nodo = getNextNodeLista(esperando);
		}while (aux->tidEsperado != tid && nodo != NULL);
		
		if(nodo==NULL){
			//so botar
			tcbAux=findTCB(tidExec);
			tcbAux->state=3;
			
			aux=malloc(sizeof(ESPERA));
			aux->tidEsperado=tid;
			aux->tidBloqueado=tidExec;
			esperando=insertLista(esperando,(void*)aux);
		
			getcontext(&(tcbAux->context));
		}else{
			return -1;
		}
	}
	if (tcbAux->state == 3) {
            dispatcher(-1);
        }
	return 	0;
}
int csem_init (csem_t *sem, int count){
    /*
    csem_init inicializa uma variável de semáforo e seu respectivo contador.

    ???????????????????????????????????????????????????????????????????????????
    like, wtf
    */
	
}
int cwait (csem_t *sem){
    /*
    cwait recebe um semáforo e checa se o contador deste semáforo é menor ou igual a zero. Se for, o processo é posto em bloqueado, esperando para acessar o recurso controlado pela variavel de semáforo. Caso contrário, o semáforo terá sua variável count decrementada e a thread poderá acessar o recurso controlado.

    -->lógica:
    república if (sem.count > 0){
        sem.count--;
        return 0;
    } else{
        TCBexec.state = bloqueado;
        semBloqueia(sem, TCBexec.tid);

        return -50000;
    }
    */
	
}
int csignal (csem_t *sem){
    /*
    csignal recebe um semáforo e incrementa sua variável de count, indicando que a thread terminou de usar o recurso controlado pelo semáforo. Nesse momento, se houver threads na lista de bloqueados do semáforo, é utilizado a ideia de FIFO, e a primeira thread na fila é posta em apto.

    -->lógica:
    if (sem.count <= 0){
        sem.count++;

        tidDesbl = semDesbloqueia(sem); //é o primeiro da fila
        TCBaux = tiraFila(tid, bloqSem); //bloqSem é a fila de bloqueados por semáforo
        appenddFila(aptos?, TCBaux);

        return 0;
    } else{
        return -900;
    }
    */
	
}
int cidentify (char *name, int size){
    /*
    Escreve nossos nomes em *name se size >= sizeOf(nomes)

    if (sizeOf(nossosNomes) <= size){
        strcpy(name, ponteiroApontandoParaNossosNomes);

        return 0;
    } else{
        return -70000;
    }
    */
	
}
