//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#ifndef ARV_B_H
#define ARV_B_H
#include "feature.h"

//constantes arvore b
#define TAM_CABECALHO_ARVORE_B 17  
#define TAM_NO_ARVORE_B 53 
#define ORDEM_ARVORE_B 4 
#define MAX_CHAVES 3
#define MAX_FILHOS 4    
#define MIN_CHAVES 1  
#define MIN_CHAVES_INTERNO 1 
#define PROMOCAO 1 
#define SEM_PROMOCAO 0 
#define ERRO -1 

typedef struct {
    char status;
    int noRaiz;
    int topo;
    int proxRRN;
    int nroNos;
} CabecalhoArvoreB;
 
typedef struct {
    char removido;
    int proximo;
    int tipoNo;
    int nroChaves;
    int C[3]; 
    int PR[3]; 
    int P[4]; 
} NoArvoreB;

int criar_arvoreB(char *nome_arquivo);
FILE* abrir_arvoreB(char *nome_arquivo, char *modo);
void fechar_arvoreB(FILE *arv, char *nome_arquivo);

void inicializa_cabecalho_arvoreB(CabecalhoArvoreB *cab);
CabecalhoArvoreB le_cabecalho_arvoreB(FILE *arv);
void escreve_cabecalho_arvoreB(FILE *arv, CabecalhoArvoreB *cab);
void atualiza_status_arvoreB(FILE *arv, char status);

void inicializa_no_arvoreB(NoArvoreB *no, int tipo);
NoArvoreB le_no_arvoreB(FILE *arv, int rrn);
int escreve_no_arvoreB(FILE *arv, int rrn, NoArvoreB *no);

int procura_posicao(NoArvoreB *no, int chave);
int inserir_arvoreB(FILE *arv, int chave, int pr);
int buscar_arvoreB(FILE *arv, int chave, int *pr);
int busca_em_no(NoArvoreB *no, int chave, int *pr);

// A única função de remoção que o RunCodes vai enxergar
int remover_arvoreB(FILE *arv, int chave);

int construir_arvoreB(FILE *arv_dados, FILE *arv_indice);

#endif