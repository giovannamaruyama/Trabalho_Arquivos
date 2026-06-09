//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "indice.h"
#include "features.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void funcionalidade_8(char *nome_bin, char *nome_indice) {
    
    //abre arquivo de dados em leitura
    FILE *arv_dados = fopen(nome_bin, "rb");
    if (arv_dados == NULL) {
        printf("Falha no processamento do arquivo\n");
        return;
    }
    
    //abre arquivo de índice em leitura
    FILE *arv_indice = abrir_arvoreB(nome_indice, "rb");
    if (arv_indice == NULL) {
        printf("Falha no processamento do arquivo\n");
        fclose(arv_dados);
        return;
    }
    
    //lê número de buscas
    int num_buscas;
    scanf("%d", &num_buscas);
    
    //processa cada busca
    for (int b = 0; b < num_buscas; b++) {
        
        //lê número de critérios para esta busca
        int num_criterios;
        scanf("%d", &num_criterios);
        
        //lê critérios
        ConjuntoCriterios criterios;
        criterios.num_criterios = 0;
        
        for (int c = 0; c < num_criterios; c++) {
            char nome_campo[50];
            char valor_str[256];
            
            //lê nome do campo
            scanf("%49s", nome_campo);
            
            //identifica qual é o campo
            TipoCampo tipo = identifica_campo(nome_campo);
            
            if (tipo == CAMPO_INVALIDO) {
                continue;
            }
            
            //lê valor trata strings entre aspas
            char primeiro_char;
            scanf("%c", &primeiro_char);  //consome espaço
            
            if (primeiro_char == '"') {
                //string entre aspas
                fgets(valor_str, sizeof(valor_str), stdin);
                //remove última aspas e newline
                valor_str[strlen(valor_str)-2] = '\0';
            } else {
                //número ou NULO
                ungetc(primeiro_char, stdin);
                scanf("%255s", valor_str);
            }
            
            //armazena critério
            criterios.criterios[criterios.num_criterios].campo = tipo;
            strncpy(criterios.criterios[criterios.num_criterios].valor_str, valor_str, MAX_TAMANHO_STRING);
            
            //converte valor se for número
            if (strcmp(valor_str, "NULO") == 0) {
                criterios.criterios[criterios.num_criterios].valor_int = -1;
                criterios.criterios[criterios.num_criterios].nulo = 1;
            } else if (tipo == CAMPO_COD_ESTACAO || tipo == CAMPO_COD_LINHA ||tipo == CAMPO_COD_PROX_ESTACAO || tipo == CAMPO_DIST_PROX_ESTACAO ||tipo == CAMPO_COD_LINHA_INTEGRA || tipo == CAMPO_COD_EST_INTEGRA) {
                criterios.criterios[criterios.num_criterios].valor_int = atoi(valor_str);
                criterios.criterios[criterios.num_criterios].nulo = 0;
            }
            
            criterios.num_criterios++;
        }
        
        //se busca é por codEstacao, usa índice, caso contrário: busca sequencial
        
        int encontrou = 0;
        
        //verifica se é busca por codEstacao único
        int cod_estacao = -1;
        
        if (criterios.num_criterios == 1 && criterios.criterios[0].campo == CAMPO_COD_ESTACAO && criterios.criterios[0].nulo == 0) {
            
            //usa índice
            cod_estacao = criterios.criterios[0].valor_int;
            
            //busca na árvore-b
            int rrn;
            if (buscar_arvoreB(arv_indice, cod_estacao, &rrn)) {
                //encontrou! lê registro
                // CORREÇÃO: ler_registro_bin retorna int e recebe ponteiro
                Registro reg;
                if (ler_registro_bin(arv_dados, &reg) != -1) {
                    
                    //verifica se registro não está removido
                    if (reg.removido == '0') {
                        //exibe registro
                        printf("%d ", reg.codEstacao);
                        
                        if (reg.tamNomeEstacao == 0) {
                            printf("NULO ");
                        } else {
                            printf("%s ", reg.nomeEstacao);
                        }
                        
                        if (reg.codLinha == -1) {
                            printf("NULO ");
                        } else {
                            printf("%d ", reg.codLinha);
                        }
                        
                        if (reg.tamNomeLinha == 0) {
                            printf("NULO ");
                        } else {
                            printf("%s ", reg.nomeLinha);
                        }
                        
                        if (reg.codProxEstacao == -1) {
                            printf("NULO ");
                        } else {
                            printf("%d ", reg.codProxEstacao);
                        }
                        
                        if (reg.distProxEstacao == -1) {
                            printf("NULO ");
                        } else {
                            printf("%d ", reg.distProxEstacao);
                        }
                        
                        if (reg.codLinhaIntegra == -1) {
                            printf("NULO ");
                        } else {
                            printf("%d ", reg.codLinhaIntegra);
                        }
                        
                        if (reg.codEstIntegra == -1) {
                            printf("NULO\n");
                        } else {
                            printf("%d\n", reg.codEstIntegra);
                        }
                        
                        encontrou = 1;
                    }
                    
                    //libera memória
                    if (reg.nomeEstacao != NULL) free(reg.nomeEstacao);
                    if (reg.nomeLinha != NULL) free(reg.nomeLinha);
                }
            }
        } else {
            //busca por qualquer outro campo, começa lendo o cabeçalho
            Cabecalho cab_dados;
            fseek(arv_dados, 0, SEEK_SET);
            fread(&cab_dados.status,          sizeof(char), 1, arv_dados);
            fread(&cab_dados.topo,            sizeof(int),  1, arv_dados);
            fread(&cab_dados.proxRRN,         sizeof(int),  1, arv_dados);
            fread(&cab_dados.nroEstacoes,     sizeof(int),  1, arv_dados);
            fread(&cab_dados.nroParesEstacao, sizeof(int),  1, arv_dados);
            
            //percorre todos os registros
            for (int rrn = 0; rrn < cab_dados.proxRRN; rrn++) {
                
                //lê registro
                // CORREÇÃO: ler_registro_bin retorna int e recebe ponteiro
                Registro reg;
                if (ler_registro_bin(arv_dados, &reg) == -1) {
                    continue;
                }
                
                //ignora registros removidos
                if (reg.removido == '1') {
                    if (reg.nomeEstacao != NULL) free(reg.nomeEstacao);
                    if (reg.nomeLinha != NULL) free(reg.nomeLinha);
                    continue;
                }
                
                //verifica se satisfaz todos os critérios
                if (satisfaz_todos_criterios(&reg, &criterios)) {
                    
                    //exibe registro
                    printf("%d ", reg.codEstacao);
                    
                    if (reg.tamNomeEstacao == 0) {
                        printf("NULO ");
                    } else {
                        printf("%s ", reg.nomeEstacao);
                    }
                    
                    if (reg.codLinha == -1) {
                        printf("NULO ");
                    } else {
                        printf("%d ", reg.codLinha);
                    }
                    
                    if (reg.tamNomeLinha == 0) {
                        printf("NULO ");
                    } else {
                        printf("%s ", reg.nomeLinha);
                    }
                    
                    if (reg.codProxEstacao == -1) {
                        printf("NULO ");
                    } else {
                        printf("%d ", reg.codProxEstacao);
                    }
                    
                    if (reg.distProxEstacao == -1) {
                        printf("NULO ");
                    } else {
                        printf("%d ", reg.distProxEstacao);
                    }
                    
                    if (reg.codLinhaIntegra == -1) {
                        printf("NULO ");
                    } else {
                        printf("%d ", reg.codLinhaIntegra);
                    }
                    
                    if (reg.codEstIntegra == -1) {
                        printf("NULO\n");
                    } else {
                        printf("%d\n", reg.codEstIntegra);
                    }
                    
                    encontrou = 1;
                }
                
                //libera memória
                if (reg.nomeEstacao != NULL) free(reg.nomeEstacao);
                if (reg.nomeLinha != NULL) free(reg.nomeLinha);
            }
        }
        
        //se não encontrou nada
        if (encontrou == 0) {
            printf("Registro inexistente.\n");
        }
    }
    
    //fecha arquivos
    fechar_arvoreB(arv_indice, nome_indice);
    fclose(arv_dados);
}