//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833#include "auxcsv.h"
#include "cabecalho.h"
#include "registro.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

//Verifica se um campo extraído do CSV eh nulo, vazio ou contém apenas espaços/quebras de linha.
int nulo_csv(char *campo) {
    if (campo == NULL) return 1;
    while (*campo == ' ' || *campo == '\t' || *campo == '\n' || *campo == '\r') {
        campo++;
    }
    return *campo == '\0';
}

//converte o char em int
int converte_csv(char *campo){
    if (nulo_csv(campo)){
        return NULO;
    }
    return atoi(campo);
}

//extrai o próximo campo da linha do CSV, separando pela vírgula
char* extrai_campo_csv(char **campo_string) {
    if (*campo_string == NULL) return NULL;
    char *inicio = *campo_string;
    char *p = strchr(inicio, ','); //Busca onde está a próxima vírgula
    
    if (p != NULL) {
        *p = '\0';             //troca a vírgula para isolar a string atual
        *campo_string = p + 1; //Avança o ponteiro para o início do próximo campo
    } else {
        *campo_string = NULL;  
    }
    return inicio;
}

int ler_linha_csv(FILE *csv, Registro *reg){
    char linha[1024];

    //Tenta ler uma linha do arquivo
    if (fgets(linha, sizeof(linha), csv) == NULL) return 0; 
    
    //remove o '\n' 
    linha[strcspn(linha, "\r\n")] = '\0';

    inicializa_registro(reg);
    char *ptr = linha;
    char *campo;
  
    campo = extrai_campo_csv(&ptr);
    reg->codEstacao = converte_csv(campo);
   
    campo = extrai_campo_csv(&ptr);
    // Verifica se o campo realmente tem texto antes de alocar memória
    if (!nulo_csv(campo)) { 
        reg->tamNomeEstacao = strlen(campo);
        reg->nomeEstacao = malloc((reg->tamNomeEstacao + 1) * sizeof(char));
        strcpy(reg->nomeEstacao, campo);
    }
    
    campo = extrai_campo_csv(&ptr);
    reg->codLinha = converte_csv(campo);

    campo = extrai_campo_csv(&ptr);
    if (!nulo_csv(campo)) {
        reg->tamNomeLinha = strlen(campo);
        reg->nomeLinha = malloc((reg->tamNomeLinha + 1) * sizeof(char));
        strcpy(reg->nomeLinha, campo);
    }

    campo = extrai_campo_csv(&ptr);
    reg->codProxEstacao = converte_csv(campo);
  
    campo = extrai_campo_csv(&ptr);
    reg->distProxEstacao = converte_csv(campo);
    
    campo = extrai_campo_csv(&ptr);
    reg->codLinhaIntegra = converte_csv(campo);
    
    campo = extrai_campo_csv(&ptr);
    reg->codEstIntegra = converte_csv(campo);
    
    return 1; 
}

// Função auxiliar para recalcular contadores ignorando registros logicamente removidos
void recalcula_contadores(FILE *bin, Cabecalho *cab) {
    rewind(bin); //Limpa flags de erro e EOF
    fseek(bin, 17, SEEK_SET); //Posiciona no byte 17
    
    Registro reg;
    NoEstacao *lista_estacoes = NULL;
    NoDupla *lista_pares = NULL;
    
    int cont_estacoes = 0;
    int cont_pares = 0;

    //Varre o arquivo reconstruindo os contadores válidos
    while (ler_registro_bin(bin, &reg)) {
        if (reg.removido == '0') {
            if (reg.tamNomeEstacao > 0 && reg.nomeEstacao != NULL) {
                inserir_estacao(&lista_estacoes, reg.nomeEstacao, &cont_estacoes);
            }
            inserir_par(&lista_pares, reg.codEstacao, reg.codProxEstacao, &cont_pares);
        }
        libera_registro(&reg);
    }

    liberar_lista_estacoes(lista_estacoes);
    liberar_lista_pares(lista_pares);

    //Atualiza o cabeçalho com os números validados
    cab->nroEstacoes = cont_estacoes;
    cab->nroParesEstacao = cont_pares;
}