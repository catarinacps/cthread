/*
 * cdata.h: arquivo de inclus�o de uso apenas na gera��o da libpithread
 *
 * Esse arquivo pode ser modificado. ENTRETANTO, deve ser utilizada a TCB fornecida
 *
 * Vers�o de 25/04/2017
 *
 */
#ifndef __cdata__
#define __cdata__

#define	STATE_CRIACAO	0
#define	STATE_APTO		1
#define	STATE_EXEC		2
#define	STATE_BLOQ		3
#define	STATE_TERMINO	4

/* NAO ALTERAR ESSA struct */
typedef struct s_TCB {
	int		tid; 			// identificador da thread
	int		state;			// estado em que a thread se encontra
							// 0: Criacao; 1: Apto; 2: Execucao; 3: Bloqueado e 4: Termino
	int 		ticket;		// prioridade associada ao processo
	ucontext_t 	context;	// contexto de execucao da thread (SP, PC, GPRs e recursos)
} TCB_t;

#endif
