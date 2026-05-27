#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>
#define TAM_REGISTRO 80
#define LIXO '$'
#define MAX_CAMPOS_BUSCA 8
#define MAX_TAMANHO_STRING 256

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

typedef enum {
    CAMPO_COD_ESTACAO,
    CAMPO_NOME_ESTACAO,
    CAMPO_COD_LINHA,
    CAMPO_NOME_LINHA,
    CAMPO_COD_PROX_ESTACAO,
    CAMPO_DIST_PROX_ESTACAO,
    CAMPO_COD_LINHA_INTEGRA,
    CAMPO_COD_EST_INTEGRA,
    CAMPO_INVALIDO //default
} TipoCampo;

typedef struct {
    TipoCampo campo;
    int valor_int;                      // o int que deve ser buscado
    char valor_str[MAX_TAMANHO_STRING]; // o char que deve ser buscado
    int nulo;                           // flag caso ele queira um valor NULO (1)
} CriteriodBusca;

typedef struct { 
    CriteriodBusca criterios[MAX_CAMPOS_BUSCA];
    int num_criterios;
} ConjuntoCriterios;

//Gerenciamento de memória e inicialização
void inicializa_registro(Registro *reg);
void libera_registro(Registro *reg);

//Leitura, Escrita e Impressão
void imprime_registro(Registro *reg);
int ler_registro_bin(FILE *bin, Registro *reg);
void escreve_registro_bin(FILE *bin, Registro *reg);
void reescreve_registro_atualizado(FILE *bin, long byte_offset, Registro *reg);
void le_novo_registro(Registro *reg);

//lógica de Busca e Critérios
TipoCampo identifica_campo(const char *nome_campo);
int satisfaz_criterio(const Registro *reg, const CriteriodBusca *criterio);
int satisfaz_todos_criterios(const Registro *reg, const ConjuntoCriterios *conjunto);
int le_criterios(ConjuntoCriterios *conjunto);

//atualizacao
void aplica_atualizacao(Registro *reg, const ConjuntoCriterios *atualizacao);
int tamanho_util_registro(const Registro *reg);

#endif 