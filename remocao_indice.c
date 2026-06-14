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
    fread(&cab.topo, sizeof(int), 1, bin);
    fread(&cab.proxRRN, sizeof(int), 1, bin);
    fread(&cab.nroEstacoes, sizeof(int), 1, bin);
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
 
    // Marca como inconsistente
    cab.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fflush(bin);
 
    atualiza_status_arvoreB(arv_indice, '0');
    fflush(arv_indice);
 
    for (int i = 0; i < num_remocoes; i++) {
        ConjuntoCriterios conjunto;
        if (le_criterios(&conjunto) != 0) break;
 
        int usa_indice = 0;
        int cod_estacao_busca = -1;
 
        for (int c = 0; c < conjunto.num_criterios; c++) {
            if (conjunto.criterios[c].campo == CAMPO_COD_ESTACAO && !conjunto.criterios[c].nulo) {
                cod_estacao_busca = conjunto.criterios[c].valor_int;
                usa_indice = 1;
                break;
            }
        }
 
        if (usa_indice) {
            //busca pelo índice
            int byte_offset;
            if (buscar_arvoreB(arv_indice, cod_estacao_busca, &byte_offset)) {
                fseek(bin, byte_offset, SEEK_SET);
                Registro reg;
                if (ler_registro_bin(bin, &reg)) {
                    if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &conjunto)) {
                        int rrn = (byte_offset - TAM_CABECALHO) / TAM_REGISTRO;
                        
                        //REMOVE NO ARQUIVO DE DADOS
                        remove_logicamente(bin, &cab, rrn);
                        
                        //REMOVE DO ÍNDICE 
                        remover_arvoreB(arv_indice, cod_estacao_busca);
                    }
                    libera_registro(&reg);
                }
            }
        } else {
            //Busca sequencial
            fseek(bin, TAM_CABECALHO, SEEK_SET);
            Registro reg;
            int rrn_contador = 0;
 
            while (ler_registro_bin(bin, &reg)) {
                // verifica se segue os criterios
                if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &conjunto)) {
                    
                    //remove do arquivo de dados
                    remove_logicamente(bin, &cab, rrn_contador);
                    
                    //remove do indice se for chave primaria
                    if (reg.codEstacao != -1) {
                        remover_arvoreB(arv_indice, reg.codEstacao);
                    }
                    
                    // Reposiciona para continuar varredura
                    fseek(bin, TAM_CABECALHO + ((long)(rrn_contador + 1) * TAM_REGISTRO), SEEK_SET);
                }
                libera_registro(&reg);
                rrn_contador++;
            }
        }
    }
 
    //recalcula contadores
    recalcula_contadores(bin, &cab);
    
    //finaliza arquivo de dados
    cab.status = '1';
    escreve_cabecalho(bin, &cab);
    fflush(bin);
    fclose(bin);
 
    //sai 
    BinarioNaTela(nome_bin);
    fechar_arvoreB(arv_indice, nome_indice);
}
