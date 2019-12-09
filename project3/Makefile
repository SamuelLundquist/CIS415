CC = gcc
CFLAGS = -Wall -std=gnu11 -lpthread
DEPS = command.h
OBJ = quacker.o

%.o: %.c $(DEPS)
	$(CC) -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

run:
	./server config.txt

clean:
	rm $(OBJ) server
