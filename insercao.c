//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "features.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void insert_into(char *nome_bin, int num_insercoes) {
    FILE *bin = fopen(nome_bin, "rb+");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    //Lê cabeçalho completo 
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
 
    cab.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fflush(bin);

 
    for (int i = 0; i < num_insercoes; i++) {
        Registro reg;
        le_novo_registro(&reg);
 
        long byte_offset_destino;
 
        if (cab.topo != -1) {
           
            int rrn_reuso = cab.topo;
            byte_offset_destino = (long)TAM_CABECALHO + (long)rrn_reuso * TAM_REGISTRO;
 
            //Lê o campo "proximo" do registro removido para atualizar o topo 
            fseek(bin, byte_offset_destino + 1 , SEEK_SET);
            int proximo_removido;
            fread(&proximo_removido, sizeof(int), 1, bin);
 
            //Avança o topo da pilha 
            cab.topo = proximo_removido;
 
            //Escreve o novo registro no espaço reutilizado 
            fseek(bin, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(bin, &reg);
 
        } else {
            //Pilha vazia: insere no final do arquivo
            byte_offset_destino = (long)TAM_CABECALHO + (long)cab.proxRRN * TAM_REGISTRO;
            fseek(bin, byte_offset_destino, SEEK_SET);
            escreve_registro_bin(bin, &reg);
            cab.proxRRN++;
        }
 
        libera_registro(&reg);
    }
 
    //Reconstrói contadores completos varrendo o arquivo todo 
    recalcula_contadores(bin, &cab);
 
    cab.status = '1';
    escreve_cabecalho(bin, &cab);
    fflush(bin);
    fclose(bin);
 
    BinarioNaTela(nome_bin);
}