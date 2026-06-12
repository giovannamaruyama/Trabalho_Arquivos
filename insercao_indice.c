#include "features.h"
#include "arvB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void funcionalidade_9(char *nome_bin, char *nome_indice, int num_insercoes) {
    //Abre arq de dados leitura e escrita
    FILE *arv_dados = fopen(nome_bin, "rb+");
    if (arv_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //Le cabecalho e checa consistencia
    Cabecalho cab_dados;
    fseek(arv_dados, 0, SEEK_SET);
    if (fread(&cab_dados.status, sizeof(char), 1, arv_dados) != 1 || cab_dados.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
    
    //Le resto do cabecalho
    fread(&cab_dados.topo, sizeof(int), 1, arv_dados);
    fread(&cab_dados.proxRRN, sizeof(int), 1, arv_dados);
    fread(&cab_dados.nroEstacoes, sizeof(int), 1, arv_dados);
    fread(&cab_dados.nroParesEstacao, sizeof(int), 1, arv_dados);

    //Abre arq de indice
    FILE *arv_indice = abrir_arvoreB(nome_indice, "r+b");
    if (arv_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }

    //Le cabecalho do indice e checa
    CabecalhoArvoreB cab_indice = le_cabecalho_arvoreB(arv_indice);
    if (cab_indice.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        fclose(arv_indice);
        return;
    }

    //Marca arqs como inconsistentes
    cab_dados.status = '0';
    fseek(arv_dados, 0, SEEK_SET);
    fwrite(&cab_dados.status, sizeof(char), 1, arv_dados);
    fflush(arv_dados);
    atualiza_status_arvoreB(arv_indice, '0');

    //Executa as insercoes
    for (int i = 0; i < num_insercoes; i++) {
        Registro reg;
        
        //Le registro da entrada
        le_novo_registro(&reg); 

        long byte_offset_destino;
        int rrn_usado;

        //Verifica se da pra reaproveitar espaco
        if (cab_dados.topo != -1) {
            //Reaproveita rrn do topo
            rrn_usado = cab_dados.topo;
            byte_offset_destino = (long)TAM_CABECALHO + (long)rrn_usado * TAM_REGISTRO;

            //Le proximo e atualiza topo
            fseek(arv_dados, byte_offset_destino + 1, SEEK_SET);
            int proximo_removido;
            fread(&proximo_removido, sizeof(int), 1, arv_dados);
            cab_dados.topo = proximo_removido;

            //Escreve registro no espaco vazio
            fseek(arv_dados, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(arv_dados, &reg); //funcao ja preenche com $
        } else {
            //Pilha vazia insere no fim
            rrn_usado = cab_dados.proxRRN;
            byte_offset_destino = (long)TAM_CABECALHO + (long)rrn_usado * TAM_REGISTRO;
            fseek(arv_dados, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(arv_dados, &reg);
            cab_dados.proxRRN++;
        }

        //Incrementa nro de estacoes
        cab_dados.nroEstacoes++;

        //Atualiza arvore b com novo registro
        inserir_arvoreB(arv_indice, reg.codEstacao, rrn_usado);

        //Libera memoria
        libera_registro(&reg);
    }

    //Volta status pra consistente
    cab_dados.status = '1';
    fseek(arv_dados, 0, SEEK_SET);
    fwrite(&cab_dados.status, sizeof(char), 1, arv_dados);
    fwrite(&cab_dados.topo, sizeof(int), 1, arv_dados);
    fwrite(&cab_dados.proxRRN, sizeof(int), 1, arv_dados);
    fwrite(&cab_dados.nroEstacoes, sizeof(int), 1, arv_dados);
    fwrite(&cab_dados.nroParesEstacao, sizeof(int), 1, arv_dados);
    fflush(arv_dados);
    fclose(arv_dados);

    //Finaliza indice
    atualiza_status_arvoreB(arv_indice, '1');
    fflush(arv_indice);
    fclose(arv_indice);

    //Exibe saida 
    BinarioNaTela(nome_bin);
    BinarioNaTela(nome_indice);
}