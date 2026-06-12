//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "indice.h"
#include "features.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void funcionalidade_8(char *nome_bin, char *nome_indice) {
    // abre arquivo de dados em leitura
    FILE *arv_dados = fopen(nome_bin, "rb");
    if (arv_dados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    // abre arquivo de índice em leitura
    FILE *arv_indice = abrir_arvoreB(nome_indice, "rb");
    if (arv_indice == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(arv_dados);
        return;
    }
    
    // lê número de buscas
    int num_buscas;
    if (scanf("%d", &num_buscas) != 1) return;
    
    // processa cada busca
    for (int b = 0; b < num_buscas; b++) {
        
        // lê número de critérios para esta busca
        int num_criterios;
        scanf("%d", &num_criterios);
        
        ConjuntoCriterios criterios;
        criterios.num_criterios = 0;
        
        for (int c = 0; c < num_criterios; c++) {
            char nome_campo[50];
            char valor_str[256];
            
            // lê nome do campo
            scanf("%49s", nome_campo);
            
            // identifica qual é o campo
            TipoCampo tipo = identifica_campo(nome_campo);
            
            //usa a função para ler a string 
            ScanQuoteString(valor_str);
            
            if (tipo == CAMPO_INVALIDO) {
                continue;
            }
            
            // armazena critério
            criterios.criterios[criterios.num_criterios].campo = tipo;
            strncpy(criterios.criterios[criterios.num_criterios].valor_str, valor_str, MAX_TAMANHO_STRING);
            
            // Se o valor lido ficou vazio, significa que era "nulo
            if (strlen(valor_str) == 0) {
                criterios.criterios[criterios.num_criterios].valor_int = -1;
                criterios.criterios[criterios.num_criterios].nulo = 1;
            } else {
                // converte valor se for número
                if (tipo == CAMPO_COD_ESTACAO || tipo == CAMPO_COD_LINHA ||
                    tipo == CAMPO_COD_PROX_ESTACAO || tipo == CAMPO_DIST_PROX_ESTACAO ||
                    tipo == CAMPO_COD_LINHA_INTEGRA || tipo == CAMPO_COD_EST_INTEGRA) {
                    criterios.criterios[criterios.num_criterios].valor_int = atoi(valor_str);
                }
                criterios.criterios[criterios.num_criterios].nulo = 0;
            }
            
            criterios.num_criterios++;
        }
        
        int encontrou = 0;
        
        // verifica se é busca por codEstacao único-usa o indice
        if (criterios.num_criterios == 1 && criterios.criterios[0].campo == CAMPO_COD_ESTACAO && criterios.criterios[0].nulo == 0) {
            
            int cod_estacao = criterios.criterios[0].valor_int;
            int rrn;
            
            if (buscar_arvoreB(arv_indice, cod_estacao, &rrn)) {
                // garante o posicionamento correto
                long byte_offset = TAM_CABECALHO + ((long)rrn * TAM_REGISTRO);
                fseek(arv_dados, byte_offset, SEEK_SET);
                
                Registro reg;
                if (ler_registro_bin(arv_dados, &reg) != -1) {
                    
                    if (reg.removido == '0') {
                        // exibe registro
                        printf("%d ", reg.codEstacao);
                        
                        if (reg.tamNomeEstacao == 0) printf("NULO ");
                        else printf("%s ", reg.nomeEstacao);
                        
                        if (reg.codLinha == -1) printf("NULO ");
                        else printf("%d ", reg.codLinha);
                        
                        if (reg.tamNomeLinha == 0) printf("NULO ");
                        else printf("%s ", reg.nomeLinha);
                        
                        if (reg.codProxEstacao == -1) printf("NULO ");
                        else printf("%d ", reg.codProxEstacao);
                        
                        if (reg.distProxEstacao == -1) printf("NULO ");
                        else printf("%d ", reg.distProxEstacao);
                        
                        if (reg.codLinhaIntegra == -1) printf("NULO ");
                        else printf("%d ", reg.codLinhaIntegra);
                        
                        if (reg.codEstIntegra == -1) printf("NULO\n");
                        else printf("%d\n", reg.codEstIntegra);
                        
                        encontrou = 1;
                    }
                    
                    if (reg.nomeEstacao != NULL) free(reg.nomeEstacao);
                    if (reg.nomeLinha != NULL) free(reg.nomeLinha);
                }
            }
        } else {
            // busca sequencial
            Cabecalho cab_dados;
            fseek(arv_dados, 0, SEEK_SET);
            fread(&cab_dados.status, sizeof(char), 1, arv_dados);
            fread(&cab_dados.topo, sizeof(int),  1, arv_dados);
            fread(&cab_dados.proxRRN, sizeof(int),  1, arv_dados);
            fread(&cab_dados.nroEstacoes,sizeof(int),  1, arv_dados);
            fread(&cab_dados.nroParesEstacao,sizeof(int),  1, arv_dados);
            
            for (int rrn = 0; rrn < cab_dados.proxRRN; rrn++) {
                
                // Força posicionamento p/ alinhar loop sequencial
                long byte_offset = TAM_CABECALHO + ((long)rrn * TAM_REGISTRO);
                fseek(arv_dados, byte_offset, SEEK_SET);

                Registro reg;
                if (ler_registro_bin(arv_dados, &reg) == -1) {
                    continue;
                }
                
                if (reg.removido == '1') {
                    if (reg.nomeEstacao != NULL) free(reg.nomeEstacao);
                    if (reg.nomeLinha != NULL) free(reg.nomeLinha);
                    continue;
                }
                
                if (satisfaz_todos_criterios(&reg, &criterios)) {
                    printf("%d ", reg.codEstacao);
                    
                    if (reg.tamNomeEstacao == 0) printf("NULO ");
                    else printf("%s ", reg.nomeEstacao);
                    
                    if (reg.codLinha == -1) printf("NULO ");
                    else printf("%d ", reg.codLinha);
                    
                    if (reg.tamNomeLinha == 0) printf("NULO ");
                    else printf("%s ", reg.nomeLinha);
                    
                    if (reg.codProxEstacao == -1) printf("NULO ");
                    else printf("%d ", reg.codProxEstacao);
                    
                    if (reg.distProxEstacao == -1) printf("NULO ");
                    else printf("%d ", reg.distProxEstacao);
                    
                    if (reg.codLinhaIntegra == -1) printf("NULO ");
                    else printf("%d ", reg.codLinhaIntegra);
                    
                    if (reg.codEstIntegra == -1) printf("NULO\n");
                    else printf("%d\n", reg.codEstIntegra);
                    
                    encontrou = 1;
                }
                
                if (reg.nomeEstacao != NULL) free(reg.nomeEstacao);
                if (reg.nomeLinha != NULL) free(reg.nomeLinha);
            }
        }
        
        if (encontrou == 0) {
            printf("Registro inexistente.\n");
        }
    }
    
    //fecha
    fclose(arv_indice);
    fclose(arv_dados);
}