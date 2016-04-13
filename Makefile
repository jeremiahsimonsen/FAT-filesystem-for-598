TARGET = poo_nuggets

OBJS = main.o

CC = gcc
CFLAGS = -c -Wall

all: poo_nugs

poo_nugs: main.o
	$(CC) main.o -o $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -f $(OBJS) $(TARGET)