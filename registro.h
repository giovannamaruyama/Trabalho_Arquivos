//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#ifndef REGISTRO_H
#define REGISTRO_H
#include <stdio.h>

//Gerenciamento de memória e inicialização
void inicializa_registro(Registro *reg);
void libera_registro(Registro *reg);

//Leitura, Escrita e Impressão
void imprime_registro(Registro *reg);
int ler_registro_bin(FILE *bin, Registro *reg);
void escreve_registro_bin(FILE *bin, Registro *reg);
void reescreve_registro_atualizado(FILE *bin, long byte_offset, Registro *reg);
void le_novo_registro(Registro *reg);
int tamanho_util_registro(const Registro *reg);

#endif 
