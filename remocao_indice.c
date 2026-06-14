//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "features.h"
#include "arvB.h"
#include "indice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void funcionalidade_10(char *nome_bin, char *nome_indice, int num_remocoes) {
    // Abre arquivo de dados rb+
    FILE *bin = fopen(nome_bin, "rb+");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
 
    // Le e valida cabecalho do arquivo de dados
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
 
    // Abre arquivo de indice r+b
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
 
    // Laco de remocoes logicas
    for (int i = 0; i < num_remocoes; i++) {
        ConjuntoCriterios conjunto;
        if (le_criterios(&conjunto) != 0) break;
 
        int usa_indice = 0;
        int cod_estacao_busca = -1;
 
        // Verifica se usa chave primaria
        for (int c = 0; c < conjunto.num_criterios; c++) {
            if (conjunto.criterios[c].campo == CAMPO_COD_ESTACAO && !conjunto.criterios[c].nulo) {
                cod_estacao_busca = conjunto.criterios[c].valor_int;
                usa_indice = 1;
                break;
            }
        }
 
        if (usa_indice) {
            // Busca indexada por offset
            int byte_offset;
            if (buscar_arvoreB(arv_indice, cod_estacao_busca, &byte_offset)) {
                fseek(bin, byte_offset, SEEK_SET);
                Registro reg;
                if (ler_registro_bin(bin, &reg)) {
                    if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &conjunto)) {
                        int rrn = (byte_offset - TAM_CABECALHO) / TAM_REGISTRO;
                        remove_logicamente(bin, &cab, rrn);
                    }
                    libera_registro(&reg);
                }
            }
        } else {
            // Busca sequencial completa
            fseek(bin, TAM_CABECALHO, SEEK_SET);
            Registro reg;
            int rrn_contador = 0;
 
            while (ler_registro_bin(bin, &reg)) {
                if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &conjunto)) {
                    remove_logicamente(bin, &cab, rrn_contador);
                    fseek(bin, TAM_CABECALHO + ((long)(rrn_contador + 1) * TAM_REGISTRO), SEEK_SET);
                }
                libera_registro(&reg);
                rrn_contador++;
            }
        }
    }
 
    // Recalcula contadores do arquivo de dados
    recalcula_contadores(bin, &cab);
    
    // Salva o status consistente no arquivo de dados
    cab.status = '1';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fwrite(&cab.topo, sizeof(int), 1, bin);
    fwrite(&cab.proxRRN, sizeof(int), 1, bin);
    fwrite(&cab.nroEstacoes, sizeof(int), 1, bin);
    fwrite(&cab.nroParesEstacao, sizeof(int), 1, bin);
    fflush(bin);
 
    // Fecha a arvore antiga desatualizada
    fclose(arv_indice);
    
    // Recria o arquivo de indice do zero
    criar_arvoreB(nome_indice);
    arv_indice = abrir_arvoreB(nome_indice, "r+b");
    
    // Reconstroi a arvore inteira lendo os dados
    construir_arvoreB(bin, arv_indice);
    
    // Fecha o arquivo de dados
    fclose(bin);
 
    // Exibe as saidas na tela
    BinarioNaTela(nome_bin);
    fechar_arvoreB(arv_indice, nome_indice);
}
