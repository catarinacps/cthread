#include "../include/support.h"
#include "../include/cdata.h"
#include "lista.c"

#include <ucontext.h>

LISTA threads, esperando, semaforos;

PFILA2 aptos[4];

