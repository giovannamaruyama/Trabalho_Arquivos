//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "../features.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//funcionalidade 1 cria a tabela
void create_table(char *nome_csv, char *nome_bin) {
    FILE *csv = fopen(nome_csv, "r"); 
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //Abre o arquivo binário para escritt (wb)
    FILE *bin = fopen(nome_bin, "wb");  
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        return;
    }

    Cabecalho cab;
    inicializa_cabecalho(&cab);
    //Escreve o cabeçalho inicial com status 0
    escreve_cabecalho(bin, &cab); 

    char buffer_desc[1024];
    //pula a primeira linha do CSV 
    if (fgets(buffer_desc, sizeof(buffer_desc), csv) == NULL) {
        fclose(csv);
        fclose(bin);
        return; 
    }

    //Listas auxiliares para contar estações e pares únicos 
    NoEstacao *lista_estacoes = NULL;
    NoDupla *lista_pares = NULL;

    Registro reg;
    //le cada linha do CSV, converte para a struct e grava no binário
    while (ler_linha_csv(csv, &reg)) {
        escreve_registro_bin(bin, &reg);
        cab.proxRRN++; //atualiza o contador de RRN no cabeçalho

        //atualiza os contadores de campos únicos usando as listas 
        if (reg.tamNomeEstacao > 0) {
            inserir_estacao(&lista_estacoes, reg.nomeEstacao, &cab.nroEstacoes);
        }
        inserir_par(&lista_pares, reg.codEstacao, reg.codProxEstacao, &cab.nroParesEstacao);

        //Libera a memória alocada para as strings do registro atual antes da próxima leitura
        libera_registro(&reg);
    }

    //Limpa a memória das estruturas auxiliares
    liberar_lista_estacoes(lista_estacoes);
    liberar_lista_pares(lista_pares);

    //finaliza o arquivo, define status como 1 e atualiza os contadores no topo do arquivo
    cab.status = '1'; 
    escreve_cabecalho(bin, &cab);

    fclose(csv);
    fclose(bin);

    BinarioNaTela(nome_bin);
}


