# Nome do executável final
EXEC = programaTrab

# Compilador e flags de compilação (Adicionado o -I. para a pasta raiz)
CC = gcc
CFLAGS = -Wall -Werror -g -I. -I./trabalho_1 -I./trabalho_introdutorio

# Pega todos os arquivos .c do diretório raiz (como o main.c)
SRCS_ROOT = $(wildcard *.c)

# Pega todos os arquivos .c de dentro da pasta trabalho_1
SRCS_TRAB1 = $(wildcard trabalho_1/*.c)

# Pega todos os arquivos .c de dentro da pasta trabalho_introdutorio
SRCS_INTRO = $(wildcard trabalho_introdutorio/*.c)

# Junta todos os arquivos fonte em uma lista só
SRCS = $(SRCS_ROOT) $(SRCS_TRAB1) $(SRCS_INTRO)

# Transforma os nomes dos arquivos .c em .o (arquivos objeto)
OBJS = $(SRCS:.c=.o)

# Regra principal que o RunCodes chama
all: $(EXEC)

# Como "linkar" o executável final
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Como compilar cada arquivo .c em um arquivo .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para rodar o programa (padrão RunCodes)
run: all
	./$(EXEC)

# Limpeza dos arquivos temporários
clean:
	rm -f *.o trabalho_1/*.o trabalho_introdutorio/*.o $(EXEC)
