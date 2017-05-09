#include <stdlib.h>
#include "support.h"
#include "cdata.h"

struct	nodoLista {
	void 	*nodo;			// Ponteiro para os dados do NODO
	struct	nodoLista *ant;	// Ponteiro para o nodo anterior
	struct	nodoLista *prox;	// Ponteiro para o nodo posterior
};
typedef struct nodoLista	LISTA;

//inicia lista
LISTA* L_init(){
	return NULL;
}

//insere na lista e retorna posição
int L_insert(LISTA *lista, void *dado){
	LISTA add*, *nodo;
	int iterator=0;
	
	nodo=lista;
	
	add=malloc(sizeof(LISTA));
	add.nodo=dado;
	add.ant=NULL;
	add.prox=NULL;
	
	if(L_vazia(lista)==1){
		*lista=add;
		return 0;
	}
	else{
		do{
		nodo=(*nodo).prox;
		i++;
		}
		while(L_vazia(nodo)==0);
		
		(*nodo).prox=add;
		(*add).ant=nodo;
		
		return i;
	}
}

//retorna 1 se a lista esta vaiza, senao retorna 0
int L_vazia(LISTA *lista){
	if(lista==NULL)
		return 1;
	else return 0;
}



