#include "../feature.h"
#include "arvB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void insert_idx(char *nome_bin, char *nome_indice, int num_insercoes) {
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

        //Reaproveita pilha
        if (cab.topo != -1) {
            int rrn_usado = cab.topo;
            byte_offset_destino = (long)TAM_CABECALHO + ((long)rrn_usado * (long)TAM_REGISTRO);

            //Le prox e att topo
            fseek(bin, byte_offset_destino + 1, SEEK_SET);
            int proximo_removido;
            fread(&proximo_removido, sizeof(int), 1, bin);
            cab.topo = proximo_removido;

            fseek(bin, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(bin, &reg);
        } else {
            //Insere no fim
            byte_offset_destino = (long)TAM_CABECALHO + ((long)cab.proxRRN * (long)TAM_REGISTRO);
            fseek(bin, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(bin, &reg);
            cab.proxRRN++;
        }

        //Insere byte offset na arvore
        inserir_arvoreB(arv_indice, reg.codEstacao, (int)byte_offset_destino);
        libera_registro(&reg);
    }

    //Reconstroi contadores completos varrendo o arquivo todo
    recalcula_contadores(bin, &cab);

    //Finaliza arq de dados
    cab.status = '1';
    escreve_cabecalho(bin, &cab);
    fflush(bin);
    fclose(bin);

    //Exibe saida
    BinarioNaTela(nome_bin);
    //Fecha arvore e exibe saida
    fechar_arvoreB(arv_indice, nome_indice);
}
