//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#ifndef FEATURES_H
#define FEATURES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
 //Constantes
#define TAM_CABECALHO 17
#define TAM_REGISTRO 80
#define LIXO '$'
#define NULO -1
#define MAX_TAMANHO_STRING 256
#define MAX_CAMPOS_BUSCA 8
 
 //Structs (trabalho 0)
typedef struct {
    char status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
} Cabecalho;
 
typedef struct {
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
} Registro;
 
typedef struct NoEstacao {
    char *nome;
    struct NoEstacao *prox;
} NoEstacao;
 
typedef struct NoDupla {
    int cod1;
    int cod2;
    struct NoDupla *prox;
} NoDupla;

//Structs de busca e atualizacao 
typedef enum {
    CAMPO_COD_ESTACAO,
    CAMPO_NOME_ESTACAO,
    CAMPO_COD_LINHA,
    CAMPO_NOME_LINHA,
    CAMPO_COD_PROX_ESTACAO,
    CAMPO_DIST_PROX_ESTACAO,
    CAMPO_COD_LINHA_INTEGRA,
    CAMPO_COD_EST_INTEGRA,
    CAMPO_INVALIDO
} TipoCampo;
 
typedef struct {
    TipoCampo campo;
    int valor_int;
    char valor_str[MAX_TAMANHO_STRING];
    int nulo;
} CriteriodBusca;
 
typedef struct {
    CriteriodBusca criterios[MAX_CAMPOS_BUSCA];
    int num_criterios;
} ConjuntoCriterios;
 
typedef struct {
    CriteriodBusca atualizacoes[MAX_CAMPOS_BUSCA];
    int num_atualizacoes;
} ConjuntoAtualizacoes;
 
//stuct da arvore-b
 
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

//Funcionalidades do trabalho 1:
void create_table(char *nome_csv, char *nome_bin) ;
void select_from(char *nome_bin);
void select_from_where(char *nome_bin, int num_buscas);
void delete_from(char *nome_bin, int num_remocoes);
void insert_into(char *nome_bin, int num_insercoes) ;
void update_table(char *nome_bin, int num_atualizacoes);
 
//Registros
 
void inicializa_registro(Registro *reg);
void libera_registro(Registro *reg);
void imprime_registro(Registro *reg);
int ler_registro_bin(FILE *bin, Registro *reg);
void escreve_registro_bin(FILE *bin, Registro *reg);
void le_novo_registro(Registro *reg);
void reescreve_registro_atualizado(FILE *bin, long byte_offset, Registro *reg);
 
//Cabeçalho
 
void inicializa_cabecalho(Cabecalho *cab);
void escreve_cabecalho(FILE *bin, Cabecalho *cab);
Cabecalho le_cabecalho(FILE *bin);
void inserir_estacao(NoEstacao **lista, char *nome_estacao, int *contador_estacoes);
void inserir_par(NoDupla **lista, int cod1, int cod2, int *contador_pares);
void liberar_lista_estacoes(NoEstacao *lista);
void liberar_lista_pares(NoDupla *lista);
 
//Funções de critérios e busca 
 
TipoCampo identifica_campo(const char *nome_campo);
int satisfaz_criterio(const Registro *reg, const CriteriodBusca *criterio);
int satisfaz_todos_criterios(const Registro *reg, const ConjuntoCriterios *conjunto);
int busca_por_cod_estacao_unico(const ConjuntoCriterios *conjunto);
int le_criterios(ConjuntoCriterios *conjunto);
int le_atualizacoes(ConjuntoAtualizacoes *conjunto);
void aplica_atualizacao(Registro *reg, const ConjuntoAtualizacoes *atualizacao);
void remove_logicamente(FILE *bin, Cabecalho *cab, int rrn_atual);
 
//Funções do auxcsv 
void BinarioNaTela(char *arquivo);
void ScanQuoteString(char *str);
int ler_linha_csv(FILE *csv, Registro *reg);
void recalcula_contadores(FILE *bin, Cabecalho *cab);
 
#endif /* FEATURES_H */