CC = gcc
CFLAGS = -Wall
DEPS = command.h
OBJ = main.o command.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

pseudo-shell: $(OBJ)
	gcc $(CLFAGS) -o $@ $^

.PHONY: run runf clean

run:
	./pseudo-shell -command

clean:
	rm *.o pseudo-shell
