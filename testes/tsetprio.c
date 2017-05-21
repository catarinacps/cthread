
/*
 *	Programa de exemplo de uso da biblioteca cthread
 *
 *	Versão 1.0 - 14/04/2016
 *
 *	Sistemas Operacionais I - www.inf.ufrgs.br
 *
 */

#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

void* func0(void) {
	printf("Eu sou nao serei impresso. hahahaha");
	return NULL;
}

void* func1(void) {
	printf("Eu fui impresso. hehehehehe");
	return NULL;
}


int main(int argc, char *argv[]) {

	int	id1;
	int i;

	ccreate(func0, (void *) NULL, 1);
	id1 = ccreate(func1, (void *) NULL, 1);
	
	csetprio(id1,0);

	printf("Eu sou a main após a criação de ID0 e ID1\n");

	cyield();

	printf("Eu sou a main voltando para terminar o programa\n");
}

