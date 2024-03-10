CC=gcc
CFLAGS=-I.
DEPS = database.h
OBJ = database.o main.o 

%.o: %.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

a.out: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
