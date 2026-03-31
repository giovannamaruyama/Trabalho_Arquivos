#include "features.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define TAM_CABECALHO 17
#define TAM_REG 80
#define LIXO '$'
#define MAX_LINHA_CSV 512
#define MAX_CAMPO_CSV 256

void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    if (mb == NULL) {
        fclose(fs);
        return;
    }

    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}

void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ;

    if (R == 'N' || R == 'n') {
        getchar();
        getchar();
        getchar();
        strcpy(str, "");
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) {
            strcpy(str, "");
        }
        getchar();
    } else if (R != EOF) {
        str[0] = R;
        scanf("%s", &str[1]);
    } else {
        strcpy(str, "");
    }
}

//Funções auxiliares
static long offset_do_rrn(int rrn) { //Calcula a posição (em bytes) no arquivo onde está um registro dado o seu RRN
    return (long)TAM_CABECALHO + (long)rrn * (long)TAM_REG;
}

void free_reg(REG *r) {
    if (r == NULL) return;
    if (r->nomeEstacao != NULL) {
        free(r->nomeEstacao);
        r->nomeEstacao = NULL;
    }
    if (r->nomeLinha != NULL) {
        free(r->nomeLinha);
        r->nomeLinha = NULL;
    }
    r->tamNomeEstacao = 0;
    r->tamNomeLinha = 0;
}

void init_reg(REG *r) {
    if (r == NULL) 
        return;
    r->removido = '1';
    r->proximo = -1;
    r->codEstacao = 0;
    r->codLinha = 0;
    r->codProxEstacao = 0;
    r->distProxEstacao = 0;
    r->codLinhaIntegra = 0;
    r->codEstIntegra = 0;

    r->tamNomeEstacao = 0;
    r->nomeEstacao = NULL;

    r->tamNomeLinha = 0;
    r->nomeLinha = NULL;
}

void init_header(CABECALHO *cab) {
    if (cab == NULL) return;
    cab->status = '0';        
    cab->topo = -1;
    cab->proxRRN = 0;
    cab->nroEstacoes = 0;
    cab->nroParesEstacao = 0;
}

//Lê um campo de CSV preservando vazios
static void read_csv_field(char **cursor, char *dest) {
    int i = 0;

    if (cursor == NULL || *cursor == NULL || dest == NULL) {
        return;
    }

    while (**cursor != ',' && **cursor != '\0' && **cursor != '\n' && **cursor != '\r') {
        dest[i++] = **cursor;
        (*cursor)++;
    }

    dest[i] = '\0';

    if (**cursor == ',') {
        (*cursor)++;
    }
}

/* Duplica string com malloc */
static char *copy_string(const char *src) { //aloca e copia uma string
    if (src == NULL) return NULL;

    size_t len = strlen(src);
    char *dst = (char *)malloc((len + 1) * sizeof(char));
    if (dst == NULL) return NULL;

    strcpy(dst, src);
    return dst;
}

//Funções do cabeçalho
void write_header(FILE *fp, CABECALHO *cab) {
    if (fp == NULL || cab == NULL) 
        return;

    char buf[TAM_CABECALHO];
    memset(buf, 0, TAM_CABECALHO);

    int pos = 0;

    buf[pos++] = cab->status;
    memcpy(buf + pos, &cab->topo, sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &cab->proxRRN, sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &cab->nroEstacoes, sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &cab->nroParesEstacao, sizeof(int)); pos += sizeof(int);

    fseek(fp, 0, SEEK_SET);
    fwrite(buf, sizeof(char), TAM_CABECALHO, fp);
}

void update_header(FILE *fp, CABECALHO *cab) {
    if (fp == NULL || cab == NULL) 
        return;
    write_header(fp, cab);
}

CABECALHO read_header(FILE *fp) {
    CABECALHO cab;
    memset(&cab, 0, sizeof(CABECALHO));

    if (fp == NULL) 
        return cab;

    char buf[TAM_CABECALHO];
    fseek(fp, 0, SEEK_SET);

    if (fread(buf, sizeof(char), TAM_CABECALHO, fp) != (size_t)TAM_CABECALHO) {
        return cab;
    }

    int pos = 0;

    cab.status = buf[pos++];
    memcpy(&cab.topo, buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&cab.proxRRN, buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&cab.nroEstacoes, buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&cab.nroParesEstacao, buf + pos, sizeof(int));

    return cab;
}

//Escrita no binário
int write_reg(FILE *fp, REG *r) {
    if (fp == NULL || r == NULL) 
        return 0;

    int tamNome  = (r->nomeEstacao == NULL) ? 0 : (int)strlen(r->nomeEstacao);
    int tamLinha = (r->nomeLinha   == NULL) ? 0 : (int)strlen(r->nomeLinha);

    int needed = 1
               + (int)sizeof(int) //proximo 
               + (int)sizeof(int) //codEstacao
               + (int)sizeof(int) //codLinha
               + (int)sizeof(int) //codProxEstacao 
               + (int)sizeof(int) //distProxEstacao
               + (int)sizeof(int) //codLinhaIntegra 
               + (int)sizeof(int) //codEstIntegra 
               + (int)sizeof(int) //tamNomeEstacao 
               + tamNome
               + (int)sizeof(int) //tamNomeLinha
               + tamLinha;

    if (needed > TAM_REG) { //para nn truncar
        return 0; 
    }

    char buf[TAM_REG];
    memset(buf, LIXO, TAM_REG);

    int pos = 0;

    buf[pos++] = r->removido;

    memcpy(buf + pos, &r->proximo, sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &r->codEstacao,sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &r->codLinha, sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &r->codProxEstacao, sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &r->distProxEstacao,sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &r->codLinhaIntegra,sizeof(int)); pos += sizeof(int);
    memcpy(buf + pos, &r->codEstIntegra, sizeof(int)); pos += sizeof(int);

    memcpy(buf + pos, &tamNome, sizeof(int));
    pos += sizeof(int);

    if (tamNome > 0) {
        memcpy(buf + pos, r->nomeEstacao, tamNome);
        pos += tamNome;
    }

    memcpy(buf + pos, &tamLinha, sizeof(int));
    pos += sizeof(int);

    if (tamLinha > 0) {
        memcpy(buf + pos, r->nomeLinha, tamLinha);
        pos += tamLinha;
    }

    return (int)(fwrite(buf, sizeof(char), TAM_REG, fp) == (size_t)TAM_REG);
}

//Le o registro do binário
REG read_reg(FILE *fp) {
    REG r;
    init_reg(&r);

    if (fp == NULL) {
        r.removido = '\0';
        return r;
    }

    char buf[TAM_REG];
    if (fread(buf, sizeof(char), TAM_REG, fp) != (size_t)TAM_REG) {
        r.removido = '\0';
        return r;
    }

    int pos = 0;

    r.removido = buf[pos++];
    memcpy(&r.proximo, buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&r.codEstacao, buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&r.codLinha, buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&r.codProxEstacao, buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&r.distProxEstacao,buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&r.codLinhaIntegra,buf + pos, sizeof(int)); pos += sizeof(int);
    memcpy(&r.codEstIntegra, buf + pos, sizeof(int)); pos += sizeof(int);

    memcpy(&r.tamNomeEstacao, buf + pos, sizeof(int));
    pos += sizeof(int);

    if (r.tamNomeEstacao > 0 && pos + r.tamNomeEstacao <= TAM_REG) {
        r.nomeEstacao = (char *)malloc((r.tamNomeEstacao + 1) * sizeof(char));
        if (r.nomeEstacao == NULL) {
            free_reg(&r);
            r.removido = '\0';
            return r;
        }
        memcpy(r.nomeEstacao, buf + pos, r.tamNomeEstacao);
        r.nomeEstacao[r.tamNomeEstacao] = '\0';
        pos += r.tamNomeEstacao;
    }

    memcpy(&r.tamNomeLinha, buf + pos, sizeof(int));
    pos += sizeof(int);

    if (r.tamNomeLinha > 0 && pos + r.tamNomeLinha <= TAM_REG) {
        r.nomeLinha = (char *)malloc((r.tamNomeLinha + 1) * sizeof(char));
        if (r.nomeLinha == NULL) {
            free_reg(&r);
            r.removido = '\0';
            return r;
        }
        memcpy(r.nomeLinha, buf + pos, r.tamNomeLinha);
        r.nomeLinha[r.tamNomeLinha] = '\0';
    }

    return r;
}

bool read_csv_reg(FILE *csv, REG *r){
    char linha[MAX_LINHA_CSV];
    char campo[MAX_CAMPO_CSV];
    char *cursor;

    if (csv == NULL || r == NULL) return false;

    if (fgets(linha, MAX_LINHA_CSV, csv) == NULL) {
        return false;
    }

    free_reg(r);
    init_reg(r);

    cursor = linha;

    //CodEstacao
    read_csv_field(&cursor, campo);
    if (campo[0] != '\0') r->codEstacao = atoi(campo);

    //NomeEstacao
    read_csv_field(&cursor, campo);
    if (campo[0] != '\0') {
        r->nomeEstacao = copy_string(campo);
        if (r->nomeEstacao == NULL) {
            free_reg(r);
            return false;
        }
        r->tamNomeEstacao = (int)strlen(r->nomeEstacao);
    }

    //CodLinha
    read_csv_field(&cursor, campo);
    if (campo[0] != '\0') r->codLinha = atoi(campo);

    //NomeLinha
    read_csv_field(&cursor, campo);
    if (campo[0] != '\0') {
        r->nomeLinha = copy_string(campo);
        if (r->nomeLinha == NULL) {
            free_reg(r);
            return false;
        }
        r->tamNomeLinha = (int)strlen(r->nomeLinha);
    }

    //CodProxEstacao
    read_csv_field(&cursor, campo);
    if (campo[0] != '\0') r->codProxEstacao = atoi(campo);

    //DistProxEstacao 
    read_csv_field(&cursor, campo);
    if (campo[0] != '\0') r->distProxEstacao = atoi(campo);

    //CodLinhaIntegra
    read_csv_field(&cursor, campo);
    if (campo[0] != '\0') r->codLinhaIntegra = atoi(campo);

    //CodEstIntegra
    read_csv_field(&cursor, campo);
    if (campo[0] != '\0') r->codEstIntegra = atoi(campo);

    return true;
}

//Funcionalidades:

bool func1(FILE *csv, FILE *bin) {
    if (csv == NULL || bin == NULL) return false;

    CABECALHO cab;
    REG r;
    char linhaCabecalho[MAX_LINHA_CSV];

    init_header(&cab);
    init_reg(&r);

    /* marca inconsistente durante a escrita */
    cab.status = '1';
    write_header(bin, &cab);

    /* pula cabeçalho do CSV */
    if (fgets(linhaCabecalho, MAX_LINHA_CSV, csv) == NULL) {
        cab.status = '0';
        update_header(bin, &cab);
        return true;
    }

    while (read_csv_reg(csv, &r)) {
        if (!write_reg(bin, &r)) {
            free_reg(&r);
            return false;
        }
        cab.proxRRN++;
        free_reg(&r);
    }

    cab.status = '0';
    update_header(bin, &cab);

    return true;
}