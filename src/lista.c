#include <stdlib.h>


struct	nodoLista {
	void 	*dados;			// Ponteiro para os dados do NODO
	struct	nodoLista *ant;	// Ponteiro para o nodo anterior
	struct	nodoLista *prox;	// Ponteiro para o nodo posterior
};
typedef struct nodoLista	LISTA;

//inicia lista
LISTA* L_init(){
	return NULL;
}

//insere no fim da lista e retorna posição
int L_insert(LISTA *lista, void *dado){
	LISTA *add, *nodo;
	int iterator=0;
	
	nodo=lista;
	
	//inicializa add
	add=malloc(sizeof(LISTA));
	add.dados=dado;
	add.ant=NULL;
	add.prox=NULL;
	
	if(L_vazia(lista)==1){	//se a lista estiver vazia coloca no inicio
		*lista=add;
		return 0;
	}
	else{
		while(nodo->prox!=NULL){//senao acha o fim
		nodo=nodo->prox;
		i++;
		}
		
		nodo->prox=add;			//e coloca no fim
		add->ant=nodo;
		
		return i+1;
	}
}

//retorna 1 se a lista esta vaiza, senao retorna 0
int L_vazia(LISTA *lista){
	if(lista==NULL)
		return 1;
	else return 0;
}

//remove da lista noda na posicao pos, retorna 1 se conseguiu, senao retorna 0
int L_remove(LISTA *lista, int pos){
	LISTA *nodo;
	nodo=lista;
	if(L_vazia(lista)==0){
		for(int i=0;i<pos;i++){
			if(nodo.prox==NULL)
				return 0;	//fim da lista -> nao ha elemento pos na lista
			else
			nodo=nodo->prox;	//pula pro proximos ate chegar em pos
		}
		nodo->ant->prox=nodo->prox;
		nodo->prox->ant=nodo->ant;	//encadeia nodos vizinhos
		free(nodo);
		return 1;
	}
	else return 0;	//lista vazia -> nao ha oq remover
}

//retorna o dado da lista, senao retorna NULL
void* L_read(LISTA *lista, int pos){
	if(L_vazia(lista)==1)
		return NULL;
	else return lista->dados;
}
//retorna o proximo nodo
LISTA* L_prox(LISTA *lista){
	return lista->prox;
}

//retorna o nodo anterior
LISTA* L_ant(LISTA *lista){
	return lista->ant;
}

//retorna o primeiro nodo da lista, senao retorna NULL
LISTA* L_first(LISTA *lista){
	LISTA *nodo;
	nodo=lista;
	if(L_vazia(nodo)==1)
		return NULL;
	else {
		while(nodo->ant!=NULL)
			nodo=nodo->ant;
		return nodo;
	}
}

//retorna o ultimo nodo da lista, senao retorna NULL
LISTA* L_last(LISTA *lista){
	LISTA *nodo;
	nodo=lista;
	if(L_vazia(nodo)==1)
		return NULL;
	else {
		while(nodo->prox!=NULL)
			nodo=nodo->prox;
		return nodo;
	}
}

//destroi a lista
void L_destroy(LISTA *lista){
	LISTA *nodo;
	nodo=lista;
	while(nodo->prox!=NULL){
		nodo=nodo->prox;
		free(nodo->ant);
	}
	free(nodo);
}



