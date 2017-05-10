#include "support.h"
#include "cdata.h"
#include <ucontext.h>
#include "lista.c"

LISTA threads, esperando, semaforos;

PFILA2 aptos[4];

