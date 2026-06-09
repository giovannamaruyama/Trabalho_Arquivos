CC = gcc
CFLAGS = -Wall -g

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: programaTrab

programaTrab: $(OBJECTS)
	$(CC) $(CFLAGS) -o programaTrab $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $

clean:
	rm -f *.o programaTrab

run: programaTrab
	./programaTrab