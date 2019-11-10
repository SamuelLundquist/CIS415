CC = gcc
CFLAGS = -Wall -std=gnu11
EXE = part1.exe part2.exe part3.exe
OBJ = iobound cpubound test

%.exe: %.c
	$(CC) -o $@ $^ $(CFLAGS)

%: %.c
	$(CC) -o $@ $^ $(CFLAGS)

all: $(EXE) $(OBJ)

clean:
	rm $(EXE) $(OBJ)
