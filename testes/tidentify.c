
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
#include <string.h>

int main(int argc, char *argv[]) {

	char string1[100], string2[200];
	
	if(cidentify(string1,100)!=0)
		printf("Sabe que a string nao eh grande o suficiente.\n");
	else printf("Erro.\n");
	if(cidentify(string2,200)==0)
		puts(string2);
	else printf("Erro.\n");
	

}

