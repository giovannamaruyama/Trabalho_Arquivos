#include "features.h"
#include "operacao_dados.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void funcionalidade_1(char *nome_csv, char *nome_bin) {
    FILE *csv = fopen(nome_csv, "r"); 
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //Abre o arquivo binário para escritt (wb)
    FILE *bin = fopen(nome_bin, "wb");  
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        return;
    }

    Cabecalho cab;
    inicializa_cabecalho(&cab);
    //Escreve o cabeçalho inicial com status 0
    escreve_cabecalho(bin, &cab); 

    char buffer_desc[1024];
    //pula a primeira linha do CSV 
    if (fgets(buffer_desc, sizeof(buffer_desc), csv) == NULL) {
        fclose(csv);
        fclose(bin);
        return; 
    }

    //Listas auxiliares para contar estações e pares únicos 
    NoEstacao *lista_estacoes = NULL;
    NoDupla *lista_pares = NULL;

    Registro reg;
    //le cada linha do CSV, converte para a struct e grava no binário
    while (ler_linha_csv(csv, &reg)) {
        escreve_registro_bin(bin, &reg);
        cab.proxRRN++; //atualiza o contador de RRN no cabeçalho

        //atualiza os contadores de campos únicos usando as listas 
        if (reg.tamNomeEstacao > 0) {
            inserir_estacao(&lista_estacoes, reg.nomeEstacao, &cab.nroEstacoes);
        }
        inserir_par(&lista_pares, reg.codEstacao, reg.codProxEstacao, &cab.nroParesEstacao);

        //Libera a memória alocada para as strings do registro atual antes da próxima leitura
        libera_registro(&reg);
    }

    //Limpa a memória das estruturas auxiliares
    liberar_lista_estacoes(lista_estacoes);
    liberar_lista_pares(lista_pares);

    //finaliza o arquivo, define status como 1 e atualiza os contadores no topo do arquivo
    cab.status = '1'; 
    escreve_cabecalho(bin, &cab);

    fclose(csv);
    fclose(bin);

    BinarioNaTela(nome_bin);
}

//funcionalidade 2: exibição dos registros ativos
void funcionalidade_2(char *nome_bin) {
    FILE *bin = fopen(nome_bin, "rb");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
 
    //Lê o cabeçalho 
    Cabecalho cab = le_cabecalho(bin);
    if (cab.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
 
    //Posiciona no início dos registros (byte 17)
    fseek(bin, TAM_CABECALHO, SEEK_SET);
 
    Registro reg;
    int registros_impressos = 0;
 
    // Varre sequencialmente todos os registros
    while (ler_registro_bin(bin, &reg)) {
        // Apenas registros ativos (removido == '0') são exibidos
        if (reg.removido == '0') {
            imprime_registro(&reg);
            registros_impressos++;
        }
        //o ponteiro já está posicionado no próximo registro.
        libera_registro(&reg);
    }
 
    if (registros_impressos == 0) {
        printf("Registro inexistente.\n");
    }
 
    fclose(bin);
}

//funcionalidade 3: busca com n criterios 
void funcionalidade_3(char *nome_bin, int num_buscas) {
    FILE *bin = fopen(nome_bin, "rb");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    
    // Lê o cabeçalho 
    Cabecalho cab = le_cabecalho(bin);
    if (cab.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }

    // Executa as N buscas solicitadas
    for (int busca_num = 0; busca_num < num_buscas; busca_num++) {
        ConjuntoCriterios conjunto;
        if (le_criterios(&conjunto) != 0) break;
        
        //Verifica se é busca por codEstacao único
        int busca_cod_estacao = busca_por_cod_estacao_unico(&conjunto);
        
        //Posiciona no início dos registros
        fseek(bin, TAM_CABECALHO, SEEK_SET);
        
        Registro reg;
        int encontrados = 0;
        
        // Varre o arquivo sequencialmente
        while (ler_registro_bin(bin, &reg)) {
            // Apenas registros ativos são considerados
            if (reg.removido == '0') {
                if (satisfaz_todos_criterios(&reg, &conjunto)) {
                    imprime_registro(&reg);
                    encontrados++;
                    
                    //para após encontrar
                    if (busca_cod_estacao) {
                        libera_registro(&reg);
                        break;
                    }
                }
            }
        
            libera_registro(&reg);
        }
        
        if (encontrados == 0) {
            printf("Registro inexistente.\n");
        }
 
        printf("\n");
    }
    
    fclose(bin);
}

void funcionalidade_4(char *nome_bin, int num_remocoes) {
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


void funcionalidade_5(char *nome_bin, int num_insercoes) {
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

void funcionalidade_6(char *nome_bin, int num_atualizacoes) {
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