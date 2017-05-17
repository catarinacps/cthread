CC = gcc
CFLAGS = -I$(IDIR) -Wall

IDIR = include
ODIR = obj
#LDIR = ../lib

#LIBS = -lm

_DEPS = auxlib.h lista.h cthread.h cdata.h support.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = cthread.o lista.o auxlib.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

libs: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 