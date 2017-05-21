#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib

all: tsetprio

tsetprio: tsetprio.c $(LIB_DIR)/libcthread.a
	$(CC) -o tsetprio tsetprio.c -L$(LIB_DIR) -lcthread -Wall
	
clean:
	rm -rf tsetprio *.o *~




