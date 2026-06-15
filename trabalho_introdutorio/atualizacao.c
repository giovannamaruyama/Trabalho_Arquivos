//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "../feature.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


void update_table(char *nome_bin, int num_atualizacoes) {
    // Abre em (leitura e escrita)
    FILE *bin = fopen(nome_bin, "rb+");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
 
    //Usa função de leitura de cabeçalho
    Cabecalho cab = le_cabecalho(bin);
    if (cab.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
 
    // Marca arquivo como inconsistente durante operação
    cab.status = '0';
    escreve_cabecalho(bin, &cab);
    fflush(bin);
 
    // Executa as n atualizações solicitadas
    for (int i = 0; i < num_atualizacoes; i++) {
        // Lê critérios de busca
        ConjuntoCriterios busca;
        if (le_criterios(&busca) != 0) break;
 
        // Lê campos a atualizar usando estrutura apropriada
        ConjuntoAtualizacoes atualizacao;
        if (le_atualizacoes(&atualizacao) != 0) break;
 
        // Varre o arquivo do início dos registros
        fseek(bin, TAM_CABECALHO, SEEK_SET);
        Registro reg;
        int rrn = 0;
        int encontrados = 0;
 
        while (ler_registro_bin(bin, &reg)) {
            // Se o registro satisfaz a busca e não está removido
            if (reg.removido == '0' && satisfaz_todos_criterios(&reg, &busca)) {
                long byte_offset = (long)TAM_CABECALHO + (long)rrn * TAM_REGISTRO;
 
                // Cria cópia para aplicar atualização com segurança
                Registro reg_tmp;
                inicializa_registro(&reg_tmp);
                reg_tmp.removido = reg.removido;
                reg_tmp.proximo = reg.proximo;
                reg_tmp.codEstacao = reg.codEstacao;
                reg_tmp.codLinha = reg.codLinha;
                reg_tmp.codProxEstacao = reg.codProxEstacao;
                reg_tmp.distProxEstacao = reg.distProxEstacao;
                reg_tmp.codLinhaIntegra = reg.codLinhaIntegra;
                reg_tmp.codEstIntegra = reg.codEstIntegra;
                reg_tmp.nomeEstacao = reg.nomeEstacao ? strdup(reg.nomeEstacao) : NULL;
                reg_tmp.tamNomeEstacao = reg.tamNomeEstacao;
                reg_tmp.nomeLinha = reg.nomeLinha ? strdup(reg.nomeLinha) : NULL;
                reg_tmp.tamNomeLinha = reg.tamNomeLinha;
 
                // Aplica as atualizações
                aplica_atualizacao(&reg_tmp, &atualizacao);
 
                //atualização in-place
                reescreve_registro_atualizado(bin, byte_offset, &reg_tmp);
 
                libera_registro(&reg_tmp);
                encontrados++;
 
                // Reposiciona para o próximo registro
                fseek(bin, TAM_CABECALHO + (long)(rrn + 1) * TAM_REGISTRO, SEEK_SET);
            }
            libera_registro(&reg);
            rrn++;
        }
        
        //Continua mesmo se não encontrou 
        if (encontrados == 0) {
            printf("Registro inexistente.\n");
        }
    }
 
    // Reconstrói contadores
    recalcula_contadores(bin, &cab);
 
    // Finaliza: restaura status '1'
    cab.status = '1';
    escreve_cabecalho(bin, &cab);
    
    fflush(bin);
    fclose(bin);
 
    BinarioNaTela(nome_bin);
}