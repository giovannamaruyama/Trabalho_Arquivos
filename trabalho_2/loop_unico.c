//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "feature.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
//Imprime uma linha de resultado da junção:
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
 
//Funcionalidade 12: Junção por loop único com índice árvore-B
void loop_unico_join(char *nome_bin1, char *nome_bin2, char *nome_indice) {
 
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
 
    // Abre arquivo2 (onde o índice aponta)
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
 
    // Abre índice árvore-B de bin2
    FILE *indice = abrir_arvoreB(nome_indice, "rb");
    if (indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin1);
        fclose(bin2);
        return;
    }
    CabecalhoArvoreB cab_idx = le_cabecalho_arvoreB(indice);
    if (cab_idx.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin1);
        fclose(bin2);
        fclose(indice);
        return;
    }
 
    int encontrou_algum = 0;
 
    // Loop externo em bin1, mas usa o índice para buscar diretamente em bin2
    //nn precisa percorrer todo o arquivo interno
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
 
        //usa a o índice para buscar reg1.codProxEstacao em bin2
        int byte_offset_bin2;
        if (buscar_arvoreB(indice, reg1.codProxEstacao, &byte_offset_bin2)) {
 
            //posiciona diretamente em bin2 pelo offset retornado
            fseek(bin2, byte_offset_bin2, SEEK_SET);
 
            Registro reg2;
            if (ler_registro_bin(bin2, &reg2)) {
                // Confirma que o registro não está removido
                if (reg2.removido == '0') {
                    imprime_juncao(&reg1, &reg2);
                    encontrou_algum = 1;
                }
                libera_registro(&reg2);
            }
        }
 
        libera_registro(&reg1);
    }
 
    if (!encontrou_algum) {
        printf("Registro inexistente.\n");
    }
 
    fclose(indice);
    fclose(bin1);
    fclose(bin2);
}