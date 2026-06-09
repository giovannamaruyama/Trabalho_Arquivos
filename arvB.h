//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833

#ifndef ARV_B_H
#define ARV_B_H
#include "features.h"

//constantes arvore b
#define TAM_CABECALHO_ARVORE_B 17    //tamanho cabeçalho: 17 bytes
#define TAM_NO_ARVORE_B 53           //Tamanho nó: 53 bytes 
#define ORDEM_ARVORE_B 4             //Ordem: n = 4 
#define MAX_CHAVES 3//Maximo chaves: 3 
#define MAX_FILHOS 4    //Maximo filhos: 4
#define MIN_CHAVES 1  //Min chaves folha


//Cria um novo arquivo de índice Árvore-B vazio
int criar_arvoreB(char *nome_arquivo);

//Abre um arquivo de índice Árvore-B existente
FILE* abrir_arvoreB(char *nome_arquivo, char *modo);

//Fecha arquivo Árvore-B
void fechar_arvoreB(FILE *arv, char *nome_arquivo);

//Operacoes de cabecalho
void inicializa_cabecalho_arvoreB(CabecalhoArvoreB *cab);

//Lê cabeçalho do arquivo da posição 0
CabecalhoArvoreB le_cabecalho_arvoreB(FILE *arv);

//Escreve cabeçalho no arquivo (posição 0)
void escreve_cabecalho_arvoreB(FILE *arv, CabecalhoArvoreB *cab);

//Atualiza apenas o status do cabeçalho
void atualiza_status_arvoreB(FILE *arv, char status);

//Funcoes para o no
void inicializa_no_arvoreB(NoArvoreB *no, int tipo);

//Lê nó do arquivo pelo RRN
NoArvoreB le_no_arvoreB(FILE *arv, int rrn);

//escreve nó no arquivo na posição RRN
int escreve_no_arvoreB(FILE *arv, int rrn, NoArvoreB *no);

//Procura posição para inserir chave em nó ordeado

int procura_posicao(NoArvoreB *no, int chave);

//Insercao
int inserir_arvoreB(FILE *arv, int chave, int pr);

int split_no_arvoreB(FILE *arv, int rrn_pai, int rrn_avo, NoArvoreB *no_cheio);

void insere_em_no(NoArvoreB *no, int chave, int pr);

//Busca
int buscar_arvoreB(FILE *arv, int chave, int *pr);

int busca_em_no(NoArvoreB *no, int chave, int *pr);

//Remocao logica
void remove_logicamente_no_arvoreB(FILE *arv, int rrn, CabecalhoArvoreB *cab);

int reutiliza_no_arvoreB(FILE *arv, CabecalhoArvoreB *cab);

//Constroi arvore
int construir_arvoreB(FILE *arv_dados, FILE *arv_indice);

#endif 
