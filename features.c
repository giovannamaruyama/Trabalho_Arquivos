#include "features.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                           // aspas! Fazer leitura normal %s então, pois deve
                           // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}

long offset_do_rrn(int rrn) {
    return (long)TAM_CABECALHO + (long)rrn * (long)TAM_REG;
}

void write_header(FILE *fp, CABECALHO *cab) {
    if (fp == NULL || cab == NULL) return;

    char buf[TAM_CABECALHO];
    memset(buf, 0, TAM_CABECALHO);  // importante
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
    if (fp == NULL || cab == NULL) return;
    write_header(fp, cab);
}

CABECALHO read_header(FILE *fp) {
    CABECALHO cab;
    memset(&cab, 0, sizeof(CABECALHO));

    if (fp == NULL) return cab;

    char buf[TAM_CABECALHO];
    fseek(fp, 0, SEEK_SET);

    if (fread(buf, sizeof(char), TAM_CABECALHO, fp) < (size_t)TAM_CABECALHO) {
        return cab; /* leitura incompleta */
    }

    int pos = 0;

    /* [0] status */
    cab.status = buf[pos];
    pos += 1;

    /* [1..4] topo */
    memcpy(&cab.topo, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [5..8] proxRRN */
    memcpy(&cab.proxRRN, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [9..12] nroEstacoes */
    memcpy(&cab.nroEstacoes, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [13..16] nroParesEstacao */
    memcpy(&cab.nroParesEstacao, buf + pos, sizeof(int));

    return cab;
}

int write_reg(FILE *fp, REG *r) {
    if (fp == NULL || r == NULL) return 0;

    /* Calcula tamanhos localmente — não modifica a struct do caller */
    int tamNome  = (r->nomeEstacao == NULL) ? 0 : (int)strlen(r->nomeEstacao);
    int tamLinha = (r->nomeLinha   == NULL) ? 0 : (int)strlen(r->nomeLinha);

    /* Verifica se o registro cabe em TAM_REG bytes */
    int needed = 1                /* removido       */
               + (int)sizeof(int) /* proximo        */
               + (int)sizeof(int) /* codEstacao     */
               + (int)sizeof(int) /* codLinha       */
               + (int)sizeof(int) /* codProxEstacao */
               + (int)sizeof(int) /* distProxEstacao*/
               + (int)sizeof(int) /* codLinhaIntegra*/
               + (int)sizeof(int) /* codEstIntegra  */
               + (int)sizeof(int) /* tamNomeEstacao */
               + tamNome
               + (int)sizeof(int) /* tamNomeLinha   */
               + tamLinha;

    if (needed > TAM_REG) return 0; /* não trunca — restrição [4] */

    /* Buffer preenchido com '$' (lixo) */
    char buf[TAM_REG];
    memset(buf, LIXO, TAM_REG);

    int pos = 0;

    /* [0] removido */
    buf[pos++] = r->removido;

    /* [1..4] proximo */
    memcpy(buf + pos, &r->proximo,         sizeof(int)); pos += sizeof(int);

    /* [5..8] codEstacao */
    memcpy(buf + pos, &r->codEstacao,      sizeof(int)); pos += sizeof(int);

    /* [9..12] codLinha */
    memcpy(buf + pos, &r->codLinha,        sizeof(int)); pos += sizeof(int);

    /* [13..16] codProxEstacao */
    memcpy(buf + pos, &r->codProxEstacao,  sizeof(int)); pos += sizeof(int);

    /* [17..20] distProxEstacao */
    memcpy(buf + pos, &r->distProxEstacao, sizeof(int)); pos += sizeof(int);

    /* [21..24] codLinhaIntegra */
    memcpy(buf + pos, &r->codLinhaIntegra, sizeof(int)); pos += sizeof(int);

    /* [25..28] codEstIntegra */
    memcpy(buf + pos, &r->codEstIntegra,   sizeof(int)); pos += sizeof(int);

    /* [29..32] tamNomeEstacao */
    memcpy(buf + pos, &tamNome,            sizeof(int)); pos += sizeof(int);

    /* [33..33+tamNome-1] nomeEstacao */
    if (tamNome > 0) {
        memcpy(buf + pos, r->nomeEstacao, tamNome);
        pos += tamNome;
    }

    /* tamNomeLinha */
    memcpy(buf + pos, &tamLinha,           sizeof(int)); pos += sizeof(int);

    /* nomeLinha */
    if (tamLinha > 0) {
        memcpy(buf + pos, r->nomeLinha, tamLinha);
        pos += tamLinha;
    }

    /* Bytes pos..79 já contêm '$' pelo memset */

    return (int)(fwrite(buf, sizeof(char), TAM_REG, fp) == (size_t)TAM_REG);
}

REG read_reg(FILE *fp){
    REG r;
    memset(&r, 0, sizeof(REG));

    if (fp == NULL) return r;

    char buf[TAM_REG];
    if (fread(buf, sizeof(char), TAM_REG, fp) < (size_t)TAM_REG) {
        r.removido = '\0'; /* sinaliza erro */
        return r;
    }

    int pos = 0;

    /* [0] removido */
    r.removido = buf[pos];
    pos += 1;

    /* [1..4] proximo */
    memcpy(&r.proximo, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [5..8] codEstacao */
    memcpy(&r.codEstacao, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [9..12] codLinha */
    memcpy(&r.codLinha, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [13..16] codProxEstacao */
    memcpy(&r.codProxEstacao, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [17..20] distProxEstacao */
    memcpy(&r.distProxEstacao, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [21..24] codLinhaIntegra */
    memcpy(&r.codLinhaIntegra, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [25..28] codEstIntegra */
    memcpy(&r.codEstIntegra, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [29..32] tamNomeEstacao */
    memcpy(&r.tamNomeEstacao, buf + pos, sizeof(int));
    pos += sizeof(int);

    /* [33..] nomeEstacao */
    if (r.tamNomeEstacao > 0 && pos + r.tamNomeEstacao <= TAM_REG) {
        r.nomeEstacao = (char *)malloc((r.tamNomeEstacao + 1) * sizeof(char));
        if (r.nomeEstacao != NULL) {
            memcpy(r.nomeEstacao, buf + pos, r.tamNomeEstacao);
            r.nomeEstacao[r.tamNomeEstacao] = '\0'; /* facilita uso com printf */
        }
        pos += r.tamNomeEstacao;
    } else {
        r.nomeEstacao    = NULL;
        r.tamNomeEstacao = 0;
    }

    /* tamNomeLinha */
    if (pos + (int)sizeof(int) <= TAM_REG) {
        memcpy(&r.tamNomeLinha, buf + pos, sizeof(int));
        pos += sizeof(int);
    }

    /* nomeLinha */
    if (r.tamNomeLinha > 0 && pos + r.tamNomeLinha <= TAM_REG) {
        r.nomeLinha = (char *)malloc((r.tamNomeLinha + 1) * sizeof(char));
        if (r.nomeLinha != NULL) {
            memcpy(r.nomeLinha, buf + pos, r.tamNomeLinha);
            r.nomeLinha[r.tamNomeLinha] = '\0';
        }
        pos += r.tamNomeLinha;
    } else {
        r.nomeLinha    = NULL;
        r.tamNomeLinha = 0;
    }

    return r;
}

void free_reg(REG *r) {
    if (r == NULL) return;

    free(r->nomeEstacao);
    r->nomeEstacao    = NULL;
    r->tamNomeEstacao = 0;

    free(r->nomeLinha);
    r->nomeLinha    = NULL;
    r->tamNomeLinha = 0;
}
