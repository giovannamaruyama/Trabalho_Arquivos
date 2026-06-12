//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "indice.h"
#include "features.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void funcionalidade_8(char *nome_bin, char *nome_indice) {
    // abre arquivo de dados em leitura
    FILE *arv_dados = fopen(nome_bin, "rb");
    if (arv_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    //verifica o status do arquivo
    Cabecalho cab_dados;
    fseek(arv_dados, 0, SEEK_SET);
    if (fread(&cab_dados.status, sizeof(char), 1, arv_dados) != 1 || cab_dados.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
    
    // abre arquivo de índice em leitura
    FILE *arv_indice = abrir_arvoreB(nome_indice, "rb");
    if (arv_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
    
    // lê número de buscas
    int num_buscas;
    if (scanf("%d", &num_buscas) != 1) {
        fclose(arv_dados);
        fclose(arv_indice);
        return;
    }
    
    // processa cada busca
    for (int b = 0; b < num_buscas; b++) {
        ConjuntoCriterios criterios;
        
        //reutiliza do trabalho 1
        if (le_criterios(&criterios) != 0) break;
        
        int encontrou = 0;
        int usa_indice = 0;
        int cod_estacao_busca = -1;
        
        // Verifica se a busca envolve a chave primária 
        for (int i = 0; i < criterios.num_criterios; i++) {
            if (criterios.criterios[i].campo == CAMPO_COD_ESTACAO && !criterios.criterios[i].nulo) {
                cod_estacao_busca = criterios.criterios[i].valor_int;
                usa_indice = 1;
                break;
            }
        }
        
        if (usa_indice) {
            //busca pela arvore b
            int rrn;
            if (buscar_arvoreB(arv_indice, cod_estacao_busca, &rrn)) {
                // posiciona no arquivo de dados pelo RRN devolvido pela árvore
                long byte_offset = TAM_CABECALHO + ((long)rrn * TAM_REGISTRO);
                fseek(arv_dados, byte_offset, SEEK_SET);
                
                Registro reg;
                // Atenção: ler_registro_bin retorna 1 em sucesso!
                if (ler_registro_bin(arv_dados, &reg)) {
                    // Além de encontrar na árvore, o registro precisa satisfazer TODOS os outros critérios e não estar removido
                    if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &criterios)) {
                        imprime_registro(&reg);
                        encontrou = 1;
                    }
                    // REUTILIZAMOS A SUA FUNÇÃO! Libera a memória com segurança.
                    libera_registro(&reg); 
                }
            }
        } else {
            //busca sequencial
            Cabecalho cab_dados;
            fseek(arv_dados, 0, SEEK_SET);
            fread(&cab_dados.status, sizeof(char), 1, arv_dados);
            fread(&cab_dados.topo, sizeof(int), 1, arv_dados);
            fread(&cab_dados.proxRRN, sizeof(int), 1, arv_dados);
            fread(&cab_dados.nroEstacoes, sizeof(int), 1, arv_dados);
            fread(&cab_dados.nroParesEstacao, sizeof(int), 1, arv_dados);
            
            for (int rrn = 0; rrn < cab_dados.proxRRN; rrn++) {
                //Garante o posicionamento 
                long byte_offset = TAM_CABECALHO + ((long)rrn * TAM_REGISTRO);
                fseek(arv_dados, byte_offset, SEEK_SET);
                
                Registro reg;
                if (ler_registro_bin(arv_dados, &reg)) {
                    if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &criterios)) {
                        imprime_registro(&reg);
                        encontrou = 1;
                    }
                    libera_registro(&reg);
                }
            }
        }
        
        if (encontrou == 0) {
            printf("Registro inexistente.\n");
        }
    }
    
    // Fechamento simples sem gravar 
    fclose(arv_indice);
    fclose(arv_dados);
}
