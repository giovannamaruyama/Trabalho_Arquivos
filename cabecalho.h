//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833#ifndef CABECALHO_H

#ifndef CABECALHO_H
#define CABECALHO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAM_CABECALHO 17 

typedef struct cabecalho{
    char status;           
    int topo;          
    int proxRRN;       
    int nroEstacoes;   
    int nroParesEstacao; 
}Cabecalho;

//Listas para armazenar os nomes de estacao e as duplas
typedef struct NoEstacao {
    char *nome;
    struct NoEstacao *prox;
} NoEstacao;

typedef struct NoDupla {
    int cod1;
    int cod2;
    struct NoDupla *prox;
} NoDupla;

//Funcoes de inicializacao e escrita
void inicializa_cabecalho(Cabecalho *cab);
void escreve_cabecalho(FILE *bin, Cabecalho *cab);

//Funcoes para armazenar os nomes de estacao e as duplas
void inserir_estacao(NoEstacao **lista, char *nome_estacao, int *contador_estacoes) ;
void inserir_par(NoDupla **lista, int cod1, int cod2, int *contador_pares);
void liberar_lista_estacoes(NoEstacao *lista);
void liberar_lista_pares(NoDupla *lista);
#endif
