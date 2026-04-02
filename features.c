#include "features.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//Estrutura auxiliar para a contagem de pares únicos
typedef struct {
    int cod1;
    int cod2;
}ParEstacao;

//Funcoes auxiliares:
void BinarioNaTela(char *arquivo) {
    FILE *fs;
    if (arquivo == NULL || !(fs = fopen(arquivo, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }

    fseek(fs, 0, SEEK_END);
    size_t fl = ftell(fs);

    fseek(fs, 0, SEEK_SET);
    unsigned char *mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    unsigned long cs = 0;
    for (unsigned long i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }

    printf("%lf\n", (cs / (double)100));

    free(mb);
    fclose(fs);
}

void ScanQuoteString(char *str) {
    char R;

    while ((R = getchar()) != EOF && isspace(R))
        ; // ignorar espaços, \r, \n...

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                           // aspas! Fazer leitura normal %s então, pois deve
                           // ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}

// Função auxiliar para converter strings do CSV para inteiros, tratando nulos
int parse_int_csv(char *token) {
    if (token == NULL || strlen(token) == 0 || token[0] == '\n' || token[0] == '\r') {
        return NULO; 
    }
    return atoi(token);
}

//Inicializa os valores do cabeçalho
void inicializa_cabecalho(Cabecalho *cab) {
    cab->status = '0'; 
    cab->topo = -1;
    cab->proxRRN = 0;
    cab->nroEstacoes = 0;
    cab->nroParesEstacao = 0;
}

//Escreve o cabeçalho no arquivo binário
void escreve_cabecalho(FILE *bin, Cabecalho *cab) {
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab->status, sizeof(char), 1, bin);
    fwrite(&cab->topo, sizeof(int), 1, bin);
    fwrite(&cab->proxRRN, sizeof(int), 1, bin);
    fwrite(&cab->nroEstacoes, sizeof(int), 1, bin);
    fwrite(&cab->nroParesEstacao, sizeof(int), 1, bin);
}

//Inicializa a estrutura do registro, essencial antes de ler uma nova linha
void inicializa_registro(Registro *reg) {
    reg->removido = '0';
    reg->proximo = -1;
    reg->codEstacao = -1;
    reg->codLinha = -1;
    reg->codProxEstacao = -1;
    reg->distProxEstacao = -1;
    reg->codLinhaIntegra = -1;
    reg->codEstIntegra = -1;
    
    reg->tamNomeEstacao = 0;
    reg->nomeEstacao = NULL; 
    
    reg->tamNomeLinha = 0;
    reg->nomeLinha = NULL;   
}

//Libera a memória alocada 
void libera_registro(Registro *reg) {
    if (reg->nomeEstacao != NULL) {
        free(reg->nomeEstacao);
        reg->nomeEstacao = NULL;
    }
    if (reg->nomeLinha != NULL) {
        free(reg->nomeLinha);
        reg->nomeLinha = NULL;
    }
}


//Lê uma linha do arquivo CSV e preenche a struct Registro
int ler_linha_csv(FILE *csv, Registro *reg){
    char linha[1024];

    // Lê a linha. Se retornar NULL, eh porque chegou no final do arquivo
    if (fgets(linha, sizeof(linha), csv) == NULL) return 0; 

    //Remove os caracteres de quebra de linha do final da string
    linha[strcspn(linha, "\r\n")] = '\0';

    //Prepara o registro (zera ints e coloca ponteiros como NULL)
    inicializa_registro(reg);
    char *ptr = linha;
    char *token;
    
    //codEstacao 
    token = strsep(&ptr, ",");
    reg->codEstacao = parse_int_csv(token);
    
    //nomeEstacao
    token = strsep(&ptr, ",");
    if (token && strlen(token) > 0) {
        reg->tamNomeEstacao = strlen(token);
        reg->nomeEstacao = malloc((reg->tamNomeEstacao + 1) * sizeof(char));
        strcpy(reg->nomeEstacao, token);
    }
    
    //codLinha
    token = strsep(&ptr, ",");
    reg->codLinha = parse_int_csv(token);
    
    //nomeLinha (Alocação dinâmica)
    token = strsep(&ptr, ",");
    if (token && strlen(token) > 0) {
        reg->tamNomeLinha = strlen(token);
        reg->nomeLinha = malloc((reg->tamNomeLinha + 1) * sizeof(char));
        strcpy(reg->nomeLinha, token);
    }
    
    //codProxEstacao
    token = strsep(&ptr, ",");
    reg->codProxEstacao = parse_int_csv(token);
    
    //distProxEstacao
    token = strsep(&ptr, ",");
    reg->distProxEstacao = parse_int_csv(token);
    
    //codLinhaIntegra
    token = strsep(&ptr, ",");
    reg->codLinhaIntegra = parse_int_csv(token);
    
    //codEstIntegra
    token = strsep(&ptr, ",");
    reg->codEstIntegra = parse_int_csv(token);
    
    return 1; 
}

//Escreve um registro no arquivo binário 
void escreve_registro_bin(FILE *bin, Registro *reg) {
    int bytes_escritos = 0;
    
    //Escreve os campos fixos (Corrigido a sintaxe do fwrite)
    bytes_escritos += fwrite(&reg->removido, sizeof(char), 1, bin);
    fwrite(&reg->proximo, sizeof(int), 1, bin);         bytes_escritos += 4;
    fwrite(&reg->codEstacao, sizeof(int), 1, bin);      bytes_escritos += 4;
    fwrite(&reg->codLinha, sizeof(int), 1, bin);        bytes_escritos += 4;
    fwrite(&reg->codProxEstacao, sizeof(int), 1, bin);  bytes_escritos += 4;
    fwrite(&reg->distProxEstacao, sizeof(int), 1, bin); bytes_escritos += 4;
    fwrite(&reg->codLinhaIntegra, sizeof(int), 1, bin); bytes_escritos += 4;
    fwrite(&reg->codEstIntegra, sizeof(int), 1, bin);   bytes_escritos += 4;
    
    //Escreve campos de tamanho variável 
    fwrite(&reg->tamNomeEstacao, sizeof(int), 1, bin);  bytes_escritos += 4;
    if (reg->tamNomeEstacao > 0 && reg->nomeEstacao != NULL) {
        fwrite(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, bin);
        bytes_escritos += reg->tamNomeEstacao;
    }
    
    fwrite(&reg->tamNomeLinha, sizeof(int), 1, bin);    bytes_escritos += 4;
    if (reg->tamNomeLinha > 0 && reg->nomeLinha != NULL) {
        fwrite(reg->nomeLinha, sizeof(char), reg->tamNomeLinha, bin);
        bytes_escritos += reg->tamNomeLinha;
    }
    
    //Preenchimento de Lixo
    char lixo = LIXO; 
    while (bytes_escritos < TAM_REGISTRO) { // TAM_REGISTRO é 80
        fwrite(&lixo, sizeof(char), 1, bin);
        bytes_escritos++;
    }
} // <- CHAVE QUE ESTAVA FALTANDO PARA FECHAR A FUNÇÃO

void funcionalidade_1(char *nome_csv, char *nome_bin) {
    FILE *csv = fopen(nome_csv, "r"); //Abre o CSV pra leitura
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *bin = fopen(nome_bin, "wb");  //Abre um arquivo binário para escrita
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        return;
    }

    //Inicializa e escreve o Cabeçalho com status inconsistente ('0')
    Cabecalho cab;
    inicializa_cabecalho(&cab);
    escreve_cabecalho(bin, &cab); 

    //Ignora a primeira linha do CSV
    char buffer_desc[1024];
    if (fgets(buffer_desc, sizeof(buffer_desc), csv) == NULL) {
        //Se o arquivo estiver vazio
        fclose(csv);
        fclose(bin);
        return; 
    }

    // Variáveis dinâmicas para contagem de exclusividades para o cabeçalho
    char **estacoes_unicas = NULL;
    int max_estacoes = 0;
    
    ParEstacao *pares_unicos = NULL;
    int max_pares = 0;

    //Le o CSV e grava nno binário
    Registro reg;
    while (ler_linha_csv(csv, &reg)) {
        escreve_registro_bin(bin, &reg);
        cab.proxRRN++; // Incrementa a contagem do próximo RRN disponível

        // Lógica de cálculo de nroEstacoes (Nomes de estação únicos)
        if (reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) {
            int existe = 0;
            for (int i = 0; i < cab.nroEstacoes; i++) {
                if (strcmp(estacoes_unicas[i], reg.nomeEstacao) == 0) {
                    existe = 1;
                    break;
                }
            }
            if (!existe) {
                if (cab.nroEstacoes == max_estacoes) {
                    max_estacoes = max_estacoes == 0 ? 50 : max_estacoes * 2;
                    estacoes_unicas = realloc(estacoes_unicas, max_estacoes * sizeof(char*));
                }
                estacoes_unicas[cab.nroEstacoes] = malloc((reg.tamNomeEstacao + 1) * sizeof(char));
                strcpy(estacoes_unicas[cab.nroEstacoes], reg.nomeEstacao);
                cab.nroEstacoes++;
            }
        }

        // Lógica de cálculo de nroParesEstacao (Pares únicos de codEstacao e codProxEstacao)
        if (reg.codEstacao != -1 && reg.codProxEstacao != -1) {
            int existe_par = 0;
            for (int i = 0; i < cab.nroParesEstacao; i++) {
                if (pares_unicos[i].cod1 == reg.codEstacao && pares_unicos[i].cod2 == reg.codProxEstacao) {
                    existe_par = 1;
                    break;
                }
            }
            if (!existe_par) {
                if (cab.nroParesEstacao == max_pares) {
                    max_pares = max_pares == 0 ? 50 : max_pares * 2;
                    pares_unicos = realloc(pares_unicos, max_pares * sizeof(ParEstacao));
                }
                pares_unicos[cab.nroParesEstacao].cod1 = reg.codEstacao;
                pares_unicos[cab.nroParesEstacao].cod2 = reg.codProxEstacao;
                cab.nroParesEstacao++;
            }
        }

        libera_registro(&reg);
    }

    //Liberação da memória temporária utilizada para as contagens
    for (int i = 0; i < cab.nroEstacoes; i++) {
        free(estacoes_unicas[i]);
    }
    free(estacoes_unicas);
    free(pares_unicos);

    //Atualiza o cabeçalho
    cab.status = '1'; 
    escreve_cabecalho(bin, &cab);

    fclose(csv);
    fclose(bin);

    BinarioNaTela(nome_bin);
}