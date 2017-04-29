#include <stdio.h>
#include "support.h"
#include "cthread.h"

TCB_t TCB;

int ccreate (void* (*start)(void*), void *arg, int prio){
    /*
    ccreate, na sua primeira chamada, checa se a TCB já foi inicializada, pois se a TCB não foi ainda inicializada indica que a thread que a chamou é a thread main, o que diz para a biblioteca que a thread main deve ser incluida no escalonador.

    Ao terminar de inicializar a thread main no TCB e incluí-la no escalonador (ou não, se a thread main já estiver incluida), a ccreate criará uma nova thread, que, por sua parte, executará uma função apontada por *start.

    -->lógica:
    if (TCB == NULL){
        //inicializa TCB com main:
        tid = 0;
        state = exec;
        prio = 0???;
        context = getcontext();
    } then {
        //adiciona para TCB a thread a ser criada
        tid = ???;
        state = apto;
        prio = ???;
        context: cria a pilha, pega o contexto e faz um makecontext da função que ela vai executar
    }
    return ????? try catch em c wtf
    */
}
int csetprio (int tid, int prio){
    /*
    csetprio altera a prioridade da thread com o identificador tid para a nova prioridade prio. Obs: thread que chamou não perde a CPU, ou seja, continua em executando.

    -->lógica:
    if (procuraNasFilas(filaProcessos, tid)){
        copiaNodo(?);
        destroiNodo(?);
        appendNodoNaFilaNovaPossivelmente();
    }
    */
}
int cyield (void){
    /*
    cyield salva o contexto da thread em execução, altera seu estado, a coloca em apto e pega a próxima thread na lista de aptos, de acordo com a prioridade da mesma.

    -->lógica:
    TCB.context = getcontext();
    ''.state = apto;
    pegaProximoEColocaEmExecuzao();
    */
}
int cjoin (int tid){
    /*
    cjoin, primeiramente, checa se a thread especificada já está sendo esperada, o que indica que essa thread não pode ser espera. Caso contrário, cjoin adiciona a thread indicada e a thread que chamou a função para a lista de esperando????. Além disso, a thread que chamou vai para o estado bloqueado, esperando o término da thread em questão.
    */
}
int csem_init (csem_t *sem, int count){
    /*
    memes
    haha
    */
}
int cwait (csem_t *sem){
    /*
    memes
    haha
    */
}
int csignal (csem_t *sem){
    /*
    memes
    haha
    */
}
int cidentify (char *name, int size){
    /*
    memes
    haha
    */
}
