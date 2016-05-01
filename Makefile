TARGET = poo_nuggets 
TARGET2 = read_file

OBJS = main.o root_dir.o
OBJS2 = read_file.o root_dir.o

CC = gcc
CFLAGS = -Wall

all: $(TARGET) $(TARGET2)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(TARGET2): $(OBJS2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(OBJS2)

root_dir.o: root_dir.c root_dir.h
	$(CC) -c $(CFLAGS) root_dir.c -o root_dir.o

clean:
	rm -f $(OBJS) $(TARGET)