//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#ifndef AUXCSV_H
#define AUXCSV_H

#include <stdio.h>
#include "cabecalho.h"
#include "registro.h"

#define NULO -1
//Fornecidas:
void BinarioNaTela(char *arquivo);
void ScanQuoteString(char *str);

//Funções de manipulação e leitura 
int nulo_csv(char *campo);
int converte_csv(char *campo);
char* extrai_campo_csv(char **campo_string);
int ler_linha_csv(FILE *csv, Registro *reg);

//Funcao auxiliar para recalcular os contadores sem os removidos
void recalcula_contadores(FILE *bin, Cabecalho *cab);

#endif
