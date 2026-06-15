//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "indice.h"
#include "../feature.h"
#include "arvB.h"
#include <string.h>

//Cria um arquivo de índice Árvore-B para um arq binário existente e para cada registro não removido do arquivo de dados, insere codEstacao

void create_idx(char *nome_arq_dados, char *nome_arq_indice) {
 
    //abre arquivo de dados em modo leitura binária
    FILE *arv_dados = fopen(nome_arq_dados, "rb");
    if (arv_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
 
    //lê cabeçalho do arquivo de dados para verificar consistência
    Cabecalho cab_dados = le_cabecalho(arv_dados);
 
    //verifica se o arquivo de dados está consistente
    if (cab_dados.status != '1') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
 
    //cria o arquivo de índice Árvore-B vazio (sobrescreve se existir)
    int criado = criar_arvoreB(nome_arq_indice);
    if (criado == 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
 
    //abre arquivo de índice para leitura e escrita binária
    FILE *arv_indice = fopen(nome_arq_indice, "r+b");
    if (arv_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
 
    //marca arquivo de índice como inconsistente durante a construção
    atualiza_status_arvoreB(arv_indice, '0');
 
    //constrói o índice inserindo um registro por vez
    int sucesso = construir_arvoreB(arv_dados, arv_indice);
    if (sucesso == 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        fclose(arv_indice);
        return;
    }
 
    //fecha arquivo de dados
    fclose(arv_dados);
 
    //marca arquivo de índice como consistente e exibe conteúdo binário e fecha a arvore b
    fechar_arvoreB(arv_indice, nome_arq_indice);
}