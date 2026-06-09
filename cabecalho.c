//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "features.h"
#include "cabecalho.h"
#include "registro.h"

void inicializa_cabecalho(Cabecalho *cab) {
    cab->status = '0'; 
    cab->topo = -1;
    cab->proxRRN = 0;
    cab->nroEstacoes = 0;
    cab->nroParesEstacao = 0;
}

void escreve_cabecalho(FILE *bin, Cabecalho *cab) {
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab->status, sizeof(char), 1, bin);
    fwrite(&cab->topo, sizeof(int), 1, bin);
    fwrite(&cab->proxRRN, sizeof(int), 1, bin);
    fwrite(&cab->nroEstacoes, sizeof(int), 1, bin);
    fwrite(&cab->nroParesEstacao, sizeof(int), 1, bin);
}

//Listas para o header:

void inserir_estacao(NoEstacao **lista, char *nome_estacao, int *contador_estacoes) {
    if (nome_estacao == NULL || strlen(nome_estacao) == 0) return;

    NoEstacao *atual = *lista;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome_estacao) == 0) return; //confeere se ja nn existe uma estacao com o mesmo nome
        atual = atual->prox;
    }
    
    NoEstacao *novo = malloc(sizeof(NoEstacao)); //vai montando a lista de estacoes
    novo->nome = malloc((strlen(nome_estacao) + 1) * sizeof(char));
    strcpy(novo->nome, nome_estacao);
    novo->prox = *lista;
    *lista = novo;
    
    (*contador_estacoes)++; 
}

void inserir_par(NoDupla **lista, int cod1, int cod2, int *contador_pares) {
    if (cod1 == -1 || cod2 == -1) return;

    NoDupla *atual = *lista;
    while (atual != NULL) {
        if (atual->cod1 == cod1 && atual->cod2 == cod2) return; //confeere se ja nn existe uma dupla de estacoes igual
        atual = atual->prox;
    }
    
    NoDupla *novo_par = malloc(sizeof(NoDupla)); //monta a lista 
    novo_par->cod1 = cod1;
    novo_par->cod2 = cod2;
    novo_par->prox = *lista;
    *lista = novo_par;
    
    (*contador_pares)++; 
}

void liberar_lista_estacoes(NoEstacao *lista) {
    NoEstacao *atual = lista;
    while (atual != NULL) {
        NoEstacao *aux = atual;
        atual = atual->prox;
        free(aux->nome);
        free(aux);
    }
}

void liberar_lista_pares(NoDupla *lista) {
    NoDupla *atual = lista;
    while (atual != NULL) {
        NoDupla *aux = atual;
        atual = atual->prox;
        free(aux);
    }
}
