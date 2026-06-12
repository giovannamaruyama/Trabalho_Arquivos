#include "features.h"
#include "arvB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void funcionalidade_9(char *nome_bin, char *nome_indice, int num_insercoes) {
    // Abre arq de dados e valida
    FILE *bin = fopen(nome_bin, "rb+");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
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
    // Abre arq de indice e valida
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
    // Marca arq como inconsistente
    cab.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fflush(bin);
    atualiza_status_arvoreB(arv_indice, '0');
    fflush(arv_indice);
    // Executa insercoes
    for (int i = 0; i < num_insercoes; i++) {
        Registro reg;
        // Inicializa registro
        inicializa_registro(&reg); 
        le_novo_registro(&reg);
        long byte_offset_destino;
        int rrn_usado;
        // Verifica reaproveitamento da pilha
        if (cab.topo != -1) {
            rrn_usado = cab.topo;
            byte_offset_destino = (long)TAM_CABECALHO + (long)rrn_usado * TAM_REGISTRO;
            // Le prox e atualiza topo 
            fseek(bin, byte_offset_destino + 1 , SEEK_SET);
            int proximo_removido;
            fread(&proximo_removido, sizeof(int), 1, bin);
            // Avanca topo da pilha 
            cab.topo = proximo_removido;
            // Escreve registro no espaco vazio 
            fseek(bin, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(bin, &reg);
        } else {
            // Pilha vazia insere no fim
            rrn_usado = cab.proxRRN;
            byte_offset_destino = (long)TAM_CABECALHO + (long)cab.proxRRN * TAM_REGISTRO;
            fseek(bin, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(bin, &reg);
            cab.proxRRN++;
        }
        cab.nroEstacoes++;
        // Atualiza indice arvore-b
        inserir_arvoreB(arv_indice, reg.codEstacao, rrn_usado);
        libera_registro(&reg);
    }
    // Fecha arvore-b
    fechar_arvoreB(arv_indice, nome_indice);
    // Atualiza cabecalho de dados
    cab.status = '1';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fwrite(&cab.topo, sizeof(int), 1, bin);
    fwrite(&cab.proxRRN, sizeof(int), 1, bin);
    fwrite(&cab.nroEstacoes, sizeof(int), 1, bin);
    fwrite(&cab.nroParesEstacao, sizeof(int), 1, bin);
    fflush(bin);
    fclose(bin);
    // Exibe saida do arquivo de dados
    BinarioNaTela(nome_bin);
}
