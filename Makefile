CC = gcc
CFLAGS = -Wall -g

# Busca os arquivos .c na raiz E dentro da pasta trabalho_introdutorio
SOURCES = $(wildcard *.c) $(wildcard trabalho_introdutorio/*.c)

# Transforma a lista de .c em .o (preservando os caminhos das pastas)
OBJECTS = $(SOURCES:.c=.o)

all: programaTrab

programaTrab: $(OBJECTS)
	$(CC) $(CFLAGS) -o programaTrab $(OBJECTS)

# Regra genérica para compilar qualquer arquivo .c em .o, não importa a pasta
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o trabalho_introdutorio/*.o programaTrab

run: programaTrab
	./programaTrab