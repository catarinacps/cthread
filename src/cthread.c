/** 
 *  -- The compact threads library --
 *       Sistemas Operacionais 1
 *              INF01142
 *               2017/1
 *
 *  Definição das funções da interface
 *
 *  @authors    Henrique Correa Pereira da Silva
 *              Nicolas Eymael da Silva
 *              Gabriel Stefaniak Niemiec
*/

#include "../include/auxlib.h"
#include <string.h>

/**
 *  Variável global de controle da inicialização das estruturas utilizadas
 *  nas funções, como as filas de aptos, a lista de threads e a lista de threads
 *  esperando a liberação de recursos.
 *  É necessário manter essa informação global, já que o usuário não necessariamente
 *  usará alguma função antes das outras (eg. inicializa um semáforo antes de criar
 *  uma thread).
 *
 *  @see initializeLib();
*/
int started = 0;

/**
 *  Função que cria uma thread que executará a função recebida pelo argumento
 *  start na prioridade definida pelo argumento prio. A thread será iniciada na
 *  fila de apto correspondente.
 *  Além disso, se for a primeira chamada, a função também deve inicializar a thread
 *  main, que terá a prioridade definida como máxima e que, além disso, não possui
 *  nenhuma diferença das outras threads (ou seja, a execução do programa não fica
 *  presa a existência da thread main).
 *
 *  @param  start   O nome da funçao que a thread executará
 *  @param  arg     Um ponteiro para uma estrutura de parâmetros, caso necessário
 *  @param  prio    Prioridade da thread a ser criada
 *  @return         O "tid", ou seja, Thread ID
 *  @see            TCB_t
*/
int ccreate(void *(*start)(void *), void *arg, int prio) {
    TCB_t *tcbMain, *tcbNew;
    int *ptTidNew;
    ucontext_t *dispContext;                // contexto do dispatcher

    if (prio < 0 || prio > 4) {             // se a prioridade não é valida dá erro
        return ERRO;
    } else if (started == 0) {              // inicializa a lib na primeira vez
        initializeLib();
        started = 1;
    }

    if (emptyTCBList()) {                   // se a lista de threads está vazia
        tcbMain = malloc(sizeof(TCB_t));    // precisamos criar a tcb da main

        tcbMain->tid = getNextTid();
        tcbMain->state = STATE_EXEC;        // estado exec
        tcbMain->ticket = 0;
        getcontext(&(tcbMain->context));

        tidExec = tcbMain->tid;

        // o bloco a seguir ajusta o contexto do dispatcher
        dispContext = malloc(sizeof(ucontext_t));
        getcontext(dispContext);
        dispContext->uc_stack.ss_sp = (char *)malloc(SIGSTKSZ);
        dispContext->uc_stack.ss_size = SIGSTKSZ;
        dispContext->uc_link = NULL;
        makecontext(dispContext, (void (*)(void))dispatcher, 1, 1);

        // o bloco a seguir define o return context da thread, que no caso é a main
        // assim, quando a main terminar, irá para o dispatcher
        tcbMain->context.uc_stack.ss_sp = (char *)malloc(SIGSTKSZ);
        tcbMain->context.uc_stack.ss_size = SIGSTKSZ;
        tcbMain->context.uc_link = dispContext;

        // adiciona a TCB_t à lista de threads
        addTCB(tcbMain);
    }

    // cria a nova tcb e a adiciona à lista de threads
    tcbNew = malloc(sizeof(TCB_t));

    getcontext(&(tcbNew->context));
    tcbNew->tid = getNextTid();
    tcbNew->state = STATE_APTO;              // estado apto
    tcbNew->ticket = prio;

    // o bloco a seguir ajusta o contexto do dispatcher
    dispContext = malloc(sizeof(ucontext_t));
    getcontext(dispContext);
    dispContext->uc_stack.ss_sp = (char *)malloc(SIGSTKSZ);
    dispContext->uc_stack.ss_size = SIGSTKSZ;
    dispContext->uc_link = NULL;
    makecontext(dispContext, (void (*)(void))dispatcher, 1, 1);

    // o bloco a seguir define o return context da thread nova
    // assim, quando a thread terminar, irá para o dispatcher
    tcbNew->context.uc_stack.ss_sp = (char *)malloc(SIGSTKSZ);
    tcbNew->context.uc_stack.ss_size = SIGSTKSZ;
    tcbNew->context.uc_link = dispContext;
    makecontext(&(tcbNew->context), (void (*)(void))start, 1, arg);

    // adiciona a thread à lista de threads
    addTCB(tcbNew);

    // e insere a nova thread na fila de aptos
    ptTidNew = malloc(sizeof(int));
    *ptTidNew = tcbNew->tid;
    AppendFila2(aptos[prio], (void *)ptTidNew);

    return tcbNew->tid;
}

/**
 *  Função que altera a prioridade de uma thread, dado o parâmetro tid e o parâmetro prio
 *  recebidos.
 *
 *  @param  tid     Identificador da thread que terá sua prioridade alterada
 *  @param  prio    Nova prioridade da thread
 *  @return         Um int representando um booleano em caso de sucesso ou erro
*/
int csetprio(int tid, int prio) {
    TCB_t *tcbAux;

    if (prio < 0 || prio > 4) {
        return ERRO;
    } // se a prioridade não é valida retorna-se erro

    if ((tcbAux = findTCB(tid))) {
        if (tcbAux->state == 1) {
            changePrioFIFO(tcbAux->ticket, prio, tid);
        } // estado apto
        tcbAux->ticket = prio;

        return OK;
    } else {
        return ERRO;
    }
}

/**
 *  Função que sinaliza ao escalonador que a thread que a chamou não necessita mais
 *  ser executada, sendo assim posta na fila de aptos.
 *
 *  @return         Um int representando um booleano em caso de sucesso ou erro
*/
int cyield(void) {
    int *ptTidExec;
    TCB_t *tcbExec;

    if ((emptyTCBList())) {
        return ERRO;
    } else {
        ptTidExec = malloc(sizeof(int));
        *ptTidExec = tidExec;

        tcbExec = findTCB(tidExec);

        // se for bem sucedida ao inserir a thread na lista de aptos
        if (AppendFila2(aptos[tcbExec->ticket], (void *)ptTidExec) ==0) {
            tcbExec->state = STATE_APTO;        // altera o estado para apto
            getcontext(&(tcbExec->context));    // define o contexto da thread NESSE ponto
        } else {
            return ERRO;
        }

        if (tcbExec->state == 1) {              // se o estado for apto (teste para quando a thread retornar ao ponto acima)
            dispatcher(-1);
        }
        return OK;
    }
}

/**
 *  Função que sinaliza a intenção de espera da thread que chamou a outra thread, definida pelo parametro tid.
 *
 *  @param  tid     Identificador da thread que será esperada
 *  @return         Um int representando um booleano em caso de sucesso ou erro
*/
int cjoin(int tid) {
    TCB_t *tcbAux;
    LISTA *nodo;
    ESPERA *aux;

    nodo = esperando;

    if (!(tcbAux = findTCB(tid))) {
        return ERRO;
    } // se a thread a ser esperada nao consta na lista de threads
    if (tcbAux->state == 4) {
        return ERRO;
    } // se a thread a ser esperada ja foi terminada

    if (emptyLista(esperando)) {
        tcbAux = findTCB(tidExec);
        tcbAux->state = STATE_BLOQ;                         // estado bloq

        aux = malloc(sizeof(ESPERA));
        aux->tidEsperado = tid;                             // se nao tem nenhuma thread esperando outra, só precisamos
        aux->tidBloqueado = tidExec;                        // bloquear a thread atual e inserir ela
        esperando = insertLista(esperando, (void *)aux);    // na lista de espera

        getcontext(&(tcbAux->context));
    } else {
        do {
            aux = (ESPERA *)nodo->dados;                    // se ja tem threads na lista, entao
                                                            // precisamos checar se alguma
            nodo = getNextNodeLista(esperando);             // delas é a mesma do parametro
                                                            // (ou seja, ja esta sendo
                                                            // esperada)
        } while (aux->tidEsperado != tid && nodo != NULL);

        if (nodo == NULL && aux->tidEsperado != tid) {
            tcbAux = findTCB(
                tidExec);
            tcbAux->state = STATE_BLOQ;                     // estado bloq

            aux = malloc(sizeof(ESPERA));                   // a thread nao está sendo esperada ainda, entao só
            aux->tidEsperado =tid;                          // precisamos bloquear a thread atual e inserir ela
            aux->tidBloqueado = tidExec;                    // na lista de espera
            esperando = insertLista(esperando, (void *)aux);

            getcontext(&(tcbAux->context));                 // define o contexto da thread NESSE ponto

        } else {
            return ERRO;                                    // thread a ser esperada ja esta sendo esperada por outra
        }
    }
    if (tcbAux->state == 3) {                               // se o estado for apto (teste para quando a thread retornar ao ponto acima)
        dispatcher(-1);
    }
    return OK;
}

/**
 *  Função que inicializa um dado semáforo pelo parâmetro sem e pelo parâmetro count.
 *
 *  @param  *sem    Ponteiro para a variável de semáforo a ser iniciada
 *  @param  count   Contador da variável de semáforo
 *  @return         Um int representando um booleano em caso de sucesso ou erro
*/
int csem_init(csem_t *sem, int count) {
    if (started == 0) {                 // inicializa a lib na primeira vez
        initializeLib();
        started = 1;
    }

    if (count >= 0) {                   // inicializa o recurso e a fila de bloqueados do semaforo
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

/**
 *  Função que utiliza o recurso sinalizado pelo semaforo dado, caso o tal recurso esteja   
 *	disponivel.
 *	Se não há recurso disponivel, a thread atual é bloqueada e inserida na 
 *	fila de bloqueados do semaforo.
 *
 *  @param  *sem    Ponteiro para a variável de semáforo que gerencia o recurso
 *  @return         Um int representando um booleano em caso de sucesso ou erro
*/
int cwait(csem_t *sem) {
    TCB_t *tcbAux;
    int *tidBloqueado;

    if (sem == NULL) {
        return ERRO;
    } else {
        if (sem->count > 0) { // se há recursos disponiveis no semáforo
            sem->count--;     // vamos utiliza-lo

            return OK;
        } else {
            tcbAux = findTCB(
                tidExec);      ///////////////////////////////////////////////
            tcbAux->state = 3; // estado bloq

            tidBloqueado =
                malloc(sizeof(int)); // se NÃO há recursos disponiveis
            *tidBloqueado = tidExec; // precisamos bloquear a thread atual

            if (AppendFila2(sem->fila, (void *)tidBloqueado) !=
                0) {         // e inseri-la na lista de bloqueados
                return ERRO; // do semáforo
            }                ///////////////////////////////////////////////

            getcontext(&(tcbAux->context));

            if (tcbAux->state == 3) { // estado bloq...time loop enemy
                dispatcher(-1);
            }
            return OK;
        }
    }
}

/**
 *  Função que libera o recurso sinalizado pelo semaforo dado.
 *  Caso alguma thread esteja na fila de bloqueados do semaforo esperando pelo
 *	recurso, esta é posta na fila de aptos.
 *
 *  @param  *sem    Ponteiro para a variável de semáforo que gerencia o recurso
 *  @return         Um int representando um booleano em caso de sucesso ou erro
*/
int csignal(csem_t *sem) {
    int tidBloqueado, *tidApto;
    TCB_t *tcbApto;

    if (sem == NULL) {
        return ERRO;
    } else {
        if (FirstFila2(sem->fila) != 0) { // nao há nenhuma thread bloqueada
                                          // nesse semaforo, então ja ta pronto
            sem->count++;                 // desaloca o recurso
            return OK;
        } else {
            tidBloqueado = *((int *)GetAtIteratorFila2(
                sem->fila));                  //////////////////////////////
            DeleteAtIteratorFila2(sem->fila); //
            // pega a primeira thread bloqueada
            tcbApto = findTCB(tidBloqueado); // da fila, coloca ela em apto
            tidApto = malloc(sizeof(int));   // e remove da fila de bloqueados
            *tidApto = tidBloqueado;         //
                                             //
            if (AppendFila2(aptos[tcbApto->ticket], (void *)tidApto) ==
                0) {                //////////////////////////////
                tcbApto->state = 1; // estado apto
                return OK;
            } else {
                return ERRO;
            }
        }
    }
}

/**
 *  Função que copia os nomes e cartoes dos desenvolvedores desta biblioteca para uma string dada.
 *	Só consegue copiar as informaçoes se o tamanho dado for grande o suficiente para elas.
 *
 *  @param  *name   Ponteiro para a area da memoria em que deve ser escrito o string
 *	@param  size    Quantidade de caracteres que podem ser escritos na string
 *  @return         Um int representando um booleano em caso de sucesso ou erro
*/
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
