//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#ifndef INDICE_H
#define INDICE_H
#include "../feature.h"
#include "arvB.h"

void create_idx(char *nome_arq_dados, char *nome_arq_indice); //funcionalidade 7- constroi indice
void select_from_idx(char *nome_bin, char *nome_indice); //funcionalidade 8 -busca com índice
void insert_idx(char *nome_bin, char *nome_indice, int num_insercoes); // funcionalidade 9 - inserção em dados e índice
void delete_from_idx(char *nome_bin, char *nome_indice, int num_remocoes); // funcionalidade 10 - remoção com índice


#endif
