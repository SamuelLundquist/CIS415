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

runf: input.txt
	./psuedo-shell -f input.txt

clean:
	rm *.o pseudo-shell
