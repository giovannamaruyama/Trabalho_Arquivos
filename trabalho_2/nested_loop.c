//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "feature.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
//Imprime uma linha de resultado da junção:
//codEstacao nomeEstacao nomeLinha codProxEstacao nomeProxEstacao
static void imprime_juncao(Registro *r1, Registro *r2) {
    // codEstacao
    if (r1->codEstacao != -1) printf("%d ", r1->codEstacao);
    else printf("NULO ");
 
    // nomeEstacao
    if (r1->tamNomeEstacao > 0 && r1->nomeEstacao != NULL) printf("%s ", r1->nomeEstacao);
    else printf("NULO ");
 
    // nomeLinha
    if (r1->tamNomeLinha > 0 && r1->nomeLinha != NULL) printf("%s ", r1->nomeLinha);
    else printf("NULO ");
 
    // codProxEstacao
    if (r1->codProxEstacao != -1) printf("%d ", r1->codProxEstacao);
    else printf("NULO ");
 
    // nomeProxEstacao (vem de r2)
    if (r2->tamNomeEstacao > 0 && r2->nomeEstacao != NULL) printf("%s", r2->nomeEstacao);
    else printf("NULO");
 
    printf("\n");
}
 
//Funcionalidade 11: Junção por loop aninhado (nested loop join)
//Para cada registro ativo de bin1, percorre TODO bin2
void nested_loop_join(char *nome_bin1, char *nome_bin2) {
 
    // Abre arquivo1 (loop externo)
    FILE *bin1 = fopen(nome_bin1, "rb");
    if (bin1 == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    Cabecalho cab1 = le_cabecalho(bin1);
    if (cab1.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin1);
        return;
    }
 
    // Abre arquivo2 (loop interno)
    FILE *bin2 = fopen(nome_bin2, "rb");
    if (bin2 == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin1);
        return;
    }
    Cabecalho cab2 = le_cabecalho(bin2);
    if (cab2.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin1);
        fclose(bin2);
        return;
    }
 
    int encontrou_algum = 0;
 
    //loop externo: cada registro de bin1
    for (int rrn1 = 0; rrn1 < cab1.proxRRN; rrn1++) {
        long offset1 = TAM_CABECALHO + ((long)rrn1 * TAM_REGISTRO);
        fseek(bin1, offset1, SEEK_SET);
 
        Registro reg1;
        if (!ler_registro_bin(bin1, &reg1)) continue;
 
        // Ignora removidos e registros sem próxima estação
        if (reg1.removido == '1' || reg1.codProxEstacao == -1) {
            libera_registro(&reg1);
            continue;
        }
 
        //Loop percorre todo bin2 do início
        for (int rrn2 = 0; rrn2 < cab2.proxRRN; rrn2++) {
            long offset2 = TAM_CABECALHO + ((long)rrn2 * TAM_REGISTRO);
            fseek(bin2, offset2, SEEK_SET);
 
            Registro reg2;
            if (!ler_registro_bin(bin2, &reg2)) continue;
 
            if (reg2.removido == '1') {
                libera_registro(&reg2);
                continue;
            }
 
            // Condição de junção
            if (reg1.codProxEstacao == reg2.codEstacao) {
                imprime_juncao(&reg1, &reg2);
                encontrou_algum = 1;
            }
 
            libera_registro(&reg2);
        }
 
        libera_registro(&reg1);
    }
 
    if (!encontrou_algum) {
        printf("Registro inexistente.\n");
    }
 
    fclose(bin1);
    fclose(bin2);
}