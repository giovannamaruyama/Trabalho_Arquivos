//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "features.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void delete_from(char *nome_bin, int num_remocoes) {
    FILE *bin = fopen(nome_bin, "rb+"); 
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cab;
    // Leitura para evitar problemas de pading
    if (fread(&cab.status, sizeof(char), 1, bin) != 1 || cab.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
    fread(&cab.topo,            sizeof(int), 1, bin);
    fread(&cab.proxRRN,         sizeof(int), 1, bin);
    fread(&cab.nroEstacoes,     sizeof(int), 1, bin);
    fread(&cab.nroParesEstacao, sizeof(int), 1, bin);

    cab.status = '0';
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab.status, sizeof(char), 1, bin);
    fflush(bin);

    for (int i = 0; i < num_remocoes; i++) {
        ConjuntoCriterios conjunto;
        if (le_criterios(&conjunto) != 0) break;

        // Volta ao byte 17 para começar a busca do zero para cada critério
        fseek(bin, TAM_CABECALHO, SEEK_SET);
        
        Registro reg;
        int rrn_contador = 0;

        
        while (ler_registro_bin(bin, &reg)) {
            if (reg.removido == '0') {
                if (satisfaz_todos_criterios(&reg, &conjunto)) {
                    remove_logicamente(bin, &cab, rrn_contador);
                    cab.nroParesEstacao--;
                    //alinha para o início do proximo registo.
                    fseek(bin, TAM_CABECALHO + ((rrn_contador + 1) * TAM_REGISTRO), SEEK_SET);
                }
            }
            libera_registro(&reg);
            rrn_contador++;
        }
    }
    recalcula_contadores(bin, &cab);
    cab.status = '1'; 
    escreve_cabecalho(bin, &cab); 
    fflush(bin);

    fclose(bin);
    BinarioNaTela(nome_bin);
}
