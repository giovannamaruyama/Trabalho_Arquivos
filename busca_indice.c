//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "indice.h"
#include "features.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void funcionalidade_8(char *nome_bin, char *nome_indice) {
    // Abre arquivo de dados rb
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
    
    // Abre indice rb
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
    
    // Le numero de buscas do terminal
    int num_buscas;
    if (scanf("%d", &num_buscas) != 1) {
        fclose(arv_dados);
        fclose(arv_indice);
        return;
    }
    
    // Processa cada busca enviada
    for (int b = 0; b < num_buscas; b++) {
        ConjuntoCriterios criterios;
        if (le_criterios(&criterios) != 0) break;
        
        int encontrou = 0;
        int usa_indice = 0;
        int cod_estacao_busca = -1;
        
        // checa se busca tem chave primaria
        for (int i = 0; i < criterios.num_criterios; i++) {
            if (criterios.criterios[i].campo == CAMPO_COD_ESTACAO && !criterios.criterios[i].nulo) {
                cod_estacao_busca = criterios.criterios[i].valor_int;
                usa_indice = 1; // ativa busca indexada
                break;
            }
        }
        
        if (usa_indice) {
            // Busca rapida por offset na arvore-b
            int byte_offset_encontrado; 
            
            if (buscar_arvoreB(arv_indice, cod_estacao_busca, &byte_offset_encontrado)) {
                fseek(arv_dados, byte_offset_encontrado, SEEK_SET); // vai direto ao ponto
                
                Registro reg;
                if (ler_registro_bin(arv_dados, &reg)) {
                    if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &criterios)) {
                        imprime_registro(&reg);
                        encontrou = 1;
                    }
                    libera_registro(&reg); 
                }
            }
        } else {
            // Busca sequencial completa
            fseek(arv_dados, 17, SEEK_SET); // pula cabecalho dados
            Registro reg;
            
            while (ler_registro_bin(arv_dados, &reg)) {
                if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &criterios)) {
                    imprime_registro(&reg);
                    encontrou = 1; // achou pelo menos um
                }
                libera_registro(&reg);
            }
        }
        
        if (encontrou == 0) {
            printf("Registro inexistente.\n");
        }
        printf("\n");
    }
    
    fclose(arv_indice);
    fclose(arv_dados);
}
