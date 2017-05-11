#include <stdlib.h>

struct nodoLista {
    void *dados;            // Ponteiro para os dados do NODO
    struct nodoLista *ant;  // Ponteiro para o nodo anterior
    struct nodoLista *prox; // Ponteiro para o nodo posterior
};
typedef struct nodoLista LISTA;

// inicia lista
LISTA * initLista() {
    return NULL;
}

// insere no fim da lista e retorna posição
int insertLista(LISTA *lista, void *dado) {
    LISTA *add, *nodo;
    int iterator = 0;

    nodo = lista;

    // inicializa add
    add = malloc(sizeof(LISTA));
    add->dados = dado;
    add->ant = NULL;
    add->prox = NULL;

    if (emptyLista(lista) == 1) { // se a lista estiver vazia coloca no inicio
        lista = add;
        return 0;
    } else {
        while (nodo->prox != NULL) { // senao acha o fim
            nodo = nodo->prox;
            iterator++;
        }

        nodo->prox = add; // e coloca no fim
        add->ant = nodo;

        return iterator + 1;
    }
}

// retorna 1 se a lista esta vaiza, senao retorna 0
int emptyLista(LISTA *lista) {
    if (lista == NULL) {
        return 1;
    } else {
        return 0;
    }
}

// remove da lista noda na posicao pos, retorna 1 se conseguiu, senao retorna 0
int removeLista(LISTA *lista, int pos) {
    LISTA *nodo;
    nodo = lista;
    int i;

    if (emptyLista(lista) == 0) {
        for (i = 0; i < pos; i++) {
            if (nodo->prox == NULL) {
                return 0;
            } // fim da lista -> nao ha elemento pos na lista
            else {
                nodo = nodo->prox;
            } // pula pro proximos ate chegar em pos
        }
        nodo->ant->prox = nodo->prox;
        nodo->prox->ant = nodo->ant; // encadeia nodos vizinhos
        free(nodo);
        return 1;
    } else {
        return 0;
    } // lista vazia -> nao ha oq remover
}

// retorna o dado da lista, senao retorna NULL
void * getNodeLista(LISTA *lista, int pos) {
    if (emptyLista(lista) == 1) {
        return NULL;
    } else {
        return lista->dados;
    }
}
// retorna o proximo nodo
LISTA * getNextNodeLista(LISTA *lista) {
    return lista->prox;
}

// retorna o nodo anterior
LISTA * getPrevNodeLista(LISTA *lista) {
    return lista->ant;
}

// retorna o primeiro nodo da lista, senao retorna NULL
LISTA * getFirstNodeLista(LISTA *lista) {
    LISTA *nodo;
    nodo = lista;

    if (emptyLista(nodo) == 1)
        return NULL;
    else {
        while (nodo->ant != NULL) {
            nodo = nodo->ant;
        }
        return nodo;
    }
}

// retorna o ultimo nodo da lista, senao retorna NULL
LISTA * getLastNodeLista(LISTA *lista) {
    LISTA *nodo;
    nodo = lista;

    if (emptyLista(nodo) == 1) {
        return NULL;
    } else {
        while (nodo->prox != NULL) {
            nodo = nodo->prox;
        }
        return nodo;
    }
}

// destroi a lista
void destroyLista(LISTA *lista) {
    LISTA *nodo;
    nodo = lista;
    
    while (nodo->prox != NULL) {
        nodo = nodo->prox;
        free(nodo->ant);
    }
    free(nodo);
}

