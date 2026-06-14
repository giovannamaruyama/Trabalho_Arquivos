#include "features.h"
#include "arvB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void funcionalidade_9(char *nome_bin, char *nome_indice, int num_insercoes) {
    //Abre arq de dados e le cabecalho
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

    //Abre arq de indice
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

    //Marca status inconsistente
    cab.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fflush(bin);
    atualiza_status_arvoreB(arv_indice, '0');
    fflush(arv_indice);

    //Executa insercoes
    for (int i = 0; i < num_insercoes; i++) {
        Registro reg;
        inicializa_registro(&reg);
        le_novo_registro(&reg); 

        long byte_offset_destino;
        int rrn_usado;

        //Reaproveita pilha
        if (cab.topo != -1) {
            rrn_usado = cab.topo;
            byte_offset_destino = 17L + ((long)rrn_usado * 80L);

            //Le prox e att topo
            fseek(bin, byte_offset_destino + 1, SEEK_SET);
            int proximo_removido;
            fread(&proximo_removido, sizeof(int), 1, bin);
            cab.topo = proximo_removido;

            fseek(bin, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(bin, &reg);
        } else {
            //Insere no fim
            rrn_usado = cab.proxRRN;
            byte_offset_destino = 17L + ((long)cab.proxRRN * 80L);
            fseek(bin, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(bin, &reg);
            cab.proxRRN++;
        }
        cab.nroEstacoes++;

        //Insere byte offset na arvore
        inserir_arvoreB(arv_indice, reg.codEstacao, (int)byte_offset_destino);
        libera_registro(&reg);
    }

    //Finaliza arq de dados
    cab.status = '1';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fwrite(&cab.topo, sizeof(int), 1, bin);
    fwrite(&cab.proxRRN, sizeof(int), 1, bin);
    fwrite(&cab.nroEstacoes, sizeof(int), 1, bin);
    fwrite(&cab.nroParesEstacao, sizeof(int), 1, bin);
    fflush(bin);
    fclose(bin);

    //Exibe saida
    BinarioNaTela(nome_bin);
    //Fecha arvore e exibe saida
    fechar_arvoreB(arv_indice, nome_indice);
}