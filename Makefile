CC = gcc
CFLAGS = -Wall -g

all: programaTrab

programaTrab: main.o features.o
	$(CC) $(CFLAGS) -o programaTrab main.o features.o

main.o: main.c features.h
	$(CC) $(CFLAGS) -c main.c

features.o: features.c features.h
	$(CC) $(CFLAGS) -c features.c

clean:
	rm -f *.o programaTrab

run: programaTrab
	./programaTrab