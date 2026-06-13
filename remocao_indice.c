//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "features.h"
#include "arvB.h"
#include "indice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void funcionalidade_10(char *nome_bin, char *nome_indice, int num_remocoes) {
    // Abre arquivo de dados
    FILE *bin = fopen(nome_bin, "rb+");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
 
    // Lê e valida cabeçalho do arquivo de dados
    Cabecalho cab;
    if (fread(&cab.status, sizeof(char), 1, bin) != 1 || cab.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
    fread(&cab.topo,            sizeof(int), 1, bin);
    fread(&cab.proxRRN,         sizeof(int), 1, bin);
    fread(&cab.nroEstacoes,     sizeof(int), 1, bin);
    fread(&cab.nroParesEstacao, sizeof(int), 1, bin);
 
    // Abre arquivo de índice
    FILE *arv_indice = abrir_arvoreB(nome_indice, "r+b");
    if (arv_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
    CabecalhoArvoreB cab_indice = le_cabecalho_arvoreB(arv_indice);
    if (cab_indice.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        fclose(arv_indice);
        return;
    }
 
    // Marca ambos como inconsistentes durante a operação
    cab.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fflush(bin);
    atualiza_status_arvoreB(arv_indice, '0');
    fflush(arv_indice);
 
    // Executa as n remoções
    for (int i = 0; i < num_remocoes; i++) {
        ConjuntoCriterios criterios;
        if (le_criterios(&criterios) != 0) break;
 
        // Verifica se a busca usa codEstacao (chave primária)
        int usa_indice = 0;
        int cod_estacao_busca = -1;
        for (int k = 0; k < criterios.num_criterios; k++) {
            if (criterios.criterios[k].campo == CAMPO_COD_ESTACAO &&
                !criterios.criterios[k].nulo) {
                cod_estacao_busca = criterios.criterios[k].valor_int;
                usa_indice = 1;
                break;
            }
        }
 
        if (usa_indice) {
            // Busca direta pelo índice
            int byte_offset;
            if (buscar_arvoreB(arv_indice, cod_estacao_busca, &byte_offset)) {
                fseek(bin, byte_offset, SEEK_SET);
                Registro reg;
                if (ler_registro_bin(bin, &reg)) {
                    if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &criterios)) {
                        // Calcula RRN a partir do byte_offset
                        int rrn = (byte_offset - TAM_CABECALHO) / TAM_REGISTRO;
                        remove_logicamente(bin, &cab, rrn);
                        // Remove do índice
                        remover_arvoreB(arv_indice, cod_estacao_busca);
                    }
                    libera_registro(&reg);
                }
            }
        } else {
            // Busca sequencial
            fseek(bin, TAM_CABECALHO, SEEK_SET);
            Registro reg;
            int rrn_contador = 0;
 
            while (ler_registro_bin(bin, &reg)) {
                if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &criterios)) {
                    int cod = reg.codEstacao;
                    remove_logicamente(bin, &cab, rrn_contador);
                    // Remove do índice se tiver chave válida
                    if (cod != -1) {
                        remover_arvoreB(arv_indice, cod);
                    }
                    // Reposiciona para continuar varredura
                    fseek(bin, TAM_CABECALHO + ((long)(rrn_contador + 1) * TAM_REGISTRO), SEEK_SET);
                }
                libera_registro(&reg);
                rrn_contador++;
            }
        }
    }
 
    // Recalcula contadores e finaliza arquivo de dados
    recalcula_contadores(bin, &cab);
    cab.status = '1';
    escreve_cabecalho(bin, &cab);
    fflush(bin);
    fclose(bin);
 
    BinarioNaTela(nome_bin);
 
    // Finaliza arquivo de índice
    fechar_arvoreB(arv_indice, nome_indice);
}