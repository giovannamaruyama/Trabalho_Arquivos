//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#ifndef JUNCAO_H
#define JUNCAO_H
#include "feature.h"
#include "arvB.h"
 
//11 Junção por loop aninhado se bin1.codProxEstacao == bin2.codEstacao
void nested_loop_join(char *nome_bin1, char *nome_bin2);
 
//12 Junção por loop único com índice árvore-B se bin1.codProxEstacao == bin2.codEstacao (usando índice de bin2)
void loop_unico_join(char *nome_bin1, char *nome_bin2, char *nome_indice);
 
//13 Ordena em memória por campo (codEstacao ou codProxEstacao)
void ordenar_arquivo(char *nome_entrada, char *campo, char *nome_saida);
 
//14 Junção por ordenação-intercalação (sort-merge join) se bin1.codProxEstacao == bin2.codEstacao
void sort_merge_join(char *nome_bin1, char *nome_bin2);
 
#endif
 