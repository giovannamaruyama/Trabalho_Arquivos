#include "features.h"
#include <stdio.h>
#include <stdlib.h>

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

void read_reg(FILE *csv){

}