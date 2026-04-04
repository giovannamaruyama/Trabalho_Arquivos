#ifndef FEATURES_H
#define FEATURES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LIXO '$'
#define NULO -1
#define TAM_REGISTRO 80    
#define TAM_CABECALHO 17   

typedef struct cabecalho{
    char status;           
    int topo;          
    int proxRRN;       
    int nroEstacoes;   
    int nroParesEstacao; 
}Cabecalho;

typedef struct registro{
    char removido;       
    int proximo;       
    int codEstacao;    
    int codLinha;     
    int codProxEstacao;
    int distProxEstacao;
    int codLinhaIntegra;
    int codEstIntegra; 
    
    int tamNomeEstacao;
    char *nomeEstacao; 
    
    int tamNomeLinha; 
    char *nomeLinha;   
}Registro;

void BinarioNaTela(char *nomeArquivoBinario);
void ScanQuoteString(char *str);

//Funcoes do header
void inicializa_cabecalho(Cabecalho *cab);
void escreve_cabecalho(FILE *bin, Cabecalho *cab);

//Funcoes dos registros
void inicializa_registro(Registro *reg);
void libera_registro(Registro *reg) ;

int ler_linha_csv(FILE *csv, Registro *reg);
void escreve_registro_bin(FILE *bin, Registro *reg);

//Funcionalidades:
void funcionalidade_1(char *nome_csv, char *nome_bin);
void funcionalidade_2(char *nome_bin);
void funcionalidade_3(char *nome_bin, int num_buscas) ;

#endif
