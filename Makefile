TARGET = poo_nuggets

OBJS = main.o root_dir.o

CC = gcc
CFLAGS = -Wall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

root_dir.o: root_dir.c root_dir.h
	$(CC) -c $(CFLAGS) root_dir.c -o root_dir.o

clean:
	rm -f $(OBJS) $(TARGET)