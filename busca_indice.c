//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "indice.h"
#include "features.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void funcionalidade_8(char *nome_bin, char *nome_indice) {
    //Abre arq de dados e checa status
    FILE *arv_dados = fopen(nome_bin, "rb");
    if (arv_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    Cabecalho cab_dados;
    fseek(arv_dados, 0, SEEK_SET);
    if (fread(&cab_dados.status, sizeof(char), 1, arv_dados) != 1 || cab_dados.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
    
    //Abre arq de indice e checa status
    FILE *arv_indice = abrir_arvoreB(nome_indice, "rb");
    if (arv_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
    CabecalhoArvoreB cab_indice = le_cabecalho_arvoreB(arv_indice);
    if (cab_indice.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        fclose(arv_indice);
        return;
    }
    
    //Le num de buscas
    int num_buscas;
    if (scanf("%d", &num_buscas) != 1) {
        fclose(arv_dados);
        fclose(arv_indice);
        return;
    }
    
    //Processa buscas
    for (int b = 0; b < num_buscas; b++) {
        ConjuntoCriterios criterios;
        if (le_criterios(&criterios) != 0) break;
        
        int encontrou = 0;
        int usa_indice = 0;
        int cod_estacao_busca = -1;
        
        //Ve se a busca usa a chave primaria
        for (int i = 0; i < criterios.num_criterios; i++)