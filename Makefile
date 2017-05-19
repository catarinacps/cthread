CC = gcc
CFLAGS = -I$(IDIR) -Wall --debug

IDIR = include
ODIR = obj
LDIR = lib
SDIR = src

TARGET = lib/libcthread.a

#LIBS = -lm

_DEPS = auxlib.h lista.h cthread.h cdata.h support.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = cthread.o lista.o auxlib.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: $(TARGET)

$(TARGET): $(OBJ) bin/support.o
	ar rcs $@ $^

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o $(IDIR)/*~ $(LDIR)/*.a *~
