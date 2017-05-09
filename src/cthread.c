#include <stdio.h>
#include "support.h"
#include "cthread.h"
#include "cdata.h"

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
    if (procuraNasFilas(filaProcessos, tid) && testaSeDerRuim(tid, prio)){
        copiaNodo(?);
        destroiNodo(?);
        appendNodoNaFilaNovaPossivelmente();

        return 0;
    } else{
        return -90000;
    }
    */
}
int cyield (void){
    /*
    cyield salva o contexto da thread em execução, altera seu estado, a coloca em apto e pega a próxima thread na lista de aptos, de acordo com a prioridade da mesma.

    -->lógica:
    if (!filaVazia(aptos)){
        TCB.context = getcontext();
        TCB.state = apto;
        pegaProximoEColocaEmExecuzao();

        return 1;
    } else{
        return -9000;
    }
    */
}
int cjoin (int tid){
    /*
    cjoin, primeiramente, checa se a thread especificada já está sendo esperada, o que indica que essa thread não pode ser esperada. Caso contrário, cjoin adiciona a thread indicada e a thread que chamou a função para a lista de esperando????. Além disso, a thread que chamou vai para o estado bloqueado, esperando o término da thread em questão.

    -->lógica:
    if (!estaNaFila(tid, esperando)){
        TCBexec.state = bloqueado;
        TCBexec.context = getcontext();

        appendFila(tid, esperando);

        return 0;
    } else{
        return -8000;
    }
    */
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
