#
# Makefile para programas exemplo
# 

CC=gcc
LIB_DIR=../lib

TARGET = tsetprio tidentify

all: $(TARGET)

tsetprio: tsetprio.c $(LIB_DIR)/libcthread.a
	$(CC) -o tsetprio tsetprio.c -L$(LIB_DIR) -lcthread -Wall
	
tidentify: tidentify.c $(LIB_DIR)/libcthread.a
	$(CC) -o tidentify tidentify.c -L$(LIB_DIR) -lcthread -Wall
	
clean:
	rm -rf $(TARGET) *.o *~




