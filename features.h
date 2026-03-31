#ifndef FEATURES_H
#define FEATURES_H

#include <stdbool.h>
#define TAM_CABECALHO 17
#define TAM_REG 80

typedef struct cabecalho {
    char status;
    int topo;
    int proxRRN;
    int nroEstacoes;
    int nroParesEstacao;
} CABECALHO;

typedef struct reg{
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
} REG;

//Funções auxiliares
long offset_do_rrn(int rrn);
void init_header(CABECALHO *cab); //inicializa o cabecalho
void read_csv_field(char **cursor, char *dest); 
char *copy_string(const char *src);
void write_header(FILE *fp, CABECALHO *cab);
void update_header(FILE *fp, CABECALHO *cab);
CABECALHO read_header(FILE *fp);
int write_reg(FILE *fp, REG *r);
void free_reg(REG *r);
void init_reg(REG *r);
REG read_reg(FILE *fp);
bool read_csv_reg(FILE *csv, REG *r);
void free_reg(REG *r);

//Funcionalidades 
void func1(FILE *csv, FILE *bin);

#endif
