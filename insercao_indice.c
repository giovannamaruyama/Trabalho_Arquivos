#include "features.h"
#include "arvB.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void funcionalidade_9(char *nome_bin, char *nome_indice, int num_insercoes) {
    //Abre arq dados rb+
    FILE *arv_dados = fopen(nome_bin, "rb+");
    if (arv_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    //Le cabecalho seguro
    Cabecalho cab_dados = le_cabecalho(arv_dados);
    if (cab_dados.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
    //Abre indice r+b
    FILE *arv_indice = abrir_arvoreB(nome_indice, "r+b");
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
    //Marca inconsistente
    cab_dados.status = '0';
    escreve_cabecalho(arv_dados, &cab_dados);
    atualiza_status_arvoreB(arv_indice, '0');
    //Executa n insercoes
    for (int i = 0; i < num_insercoes; i++) {
        Registro reg;
        //Inicializa reg c/ padrao
        inicializa_registro(&reg);
        le_novo_registro(&reg); 
        long byte_offset_destino;
        int rrn_usado;
        //Verifica removidos na pilha
        if (cab_dados.topo != -1) {
            //Reaproveita rrn topo
            rrn_usado = cab_dados.topo;
            byte_offset_destino = (long)TAM_CABECALHO + (long)rrn_usado * TAM_REGISTRO;
            //Le prox e att topo
            fseek(arv_dados, byte_offset_destino + 1, SEEK_SET);
            int proximo_removido;
            fread(&proximo_removido, sizeof(int), 1, arv_dados);
            cab_dados.topo = proximo_removido;
            //Escreve por cima
            fseek(arv_dados, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(arv_dados, &reg);
        } else {
            //Pilha vazia insere no fim
            rrn_usado = cab_dados.proxRRN;
            byte_offset_destino = (long)TAM_CABECALHO + (long)rrn_usado * TAM_REGISTRO;
            fseek(arv_dados, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(arv_dados, &reg);
            cab_dados.proxRRN++;
        }
        //Incrementa estacoes ativas
        cab_dados.nroEstacoes++;
        //Att arvore-b
        inserir_arvoreB(arv_indice, reg.codEstacao, rrn_usado);
        //Libera memoria
        libera_registro(&reg);
    }
    //Finaliza status 1 arq dados
    cab_dados.status = '1';
    escreve_cabecalho(arv_dados, &cab_dados);
    fclose(arv_dados);
    //Exibe soma bytes
    BinarioNaTela(nome_bin);
    //Fecha arv e chama binario
    fechar_arvoreB(arv_indice, nome_indice);
}
