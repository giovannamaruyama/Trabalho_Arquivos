//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "features.h"
#include "cabecalho.h"
#include "registro.h"

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

void imprime_registro(Registro *reg) {
    if (reg->codEstacao != -1) printf("%d ", reg->codEstacao);
    else printf("NULO ");

    if (reg->tamNomeEstacao > 0 && reg->nomeEstacao != NULL) printf("%s ", reg->nomeEstacao);
    else printf("NULO ");
    
    if (reg->codLinha != -1) printf("%d ", reg->codLinha);
    else printf("NULO ");
    
    if (reg->tamNomeLinha > 0 && reg->nomeLinha != NULL) printf("%s ", reg->nomeLinha);
    else printf("NULO ");
    
    if (reg->codProxEstacao != -1) printf("%d ", reg->codProxEstacao);
    else printf("NULO ");
    
    if (reg->distProxEstacao != -1) printf("%d ", reg->distProxEstacao);
    else printf("NULO ");
    
    if (reg->codLinhaIntegra != -1) printf("%d ", reg->codLinhaIntegra);
    else printf("NULO ");
    
    if (reg->codEstIntegra != -1) printf("%d\n", reg->codEstIntegra); //O último campo tem  \n
    else printf("NULO\n");
}

//Função que le um registro do arquivo binário usando um buffer de 80 bytes
int ler_registro_bin(FILE *bin, Registro *reg) {
    char buffer[TAM_REGISTRO]; // Buffer de 80 bytes
    
    //Le um bloco de 80 bytes, se nn conseguir eh o fim de arquivo 
    if (fread(buffer, 1, TAM_REGISTRO, bin) != TAM_REGISTRO) {
        return 0; 
    }

    inicializa_registro(reg);
    int offset = 0; //Vai rastreando a posição dentro do buffer

    reg->removido = buffer[offset];
    offset += 1;

    memcpy(&reg->proximo, buffer + offset, 4);
    offset += 4;

    memcpy(&reg->codEstacao, buffer + offset, 4);
    offset += 4;

    memcpy(&reg->codLinha, buffer + offset, 4);
    offset += 4;

    memcpy(&reg->codProxEstacao, buffer + offset, 4);
    offset += 4;

    memcpy(&reg->distProxEstacao, buffer + offset, 4);
    offset += 4;

    memcpy(&reg->codLinhaIntegra, buffer + offset, 4);
    offset += 4;

    memcpy(&reg->codEstIntegra, buffer + offset, 4);
    offset += 4;

    memcpy(&reg->tamNomeEstacao, buffer + offset, 4);
    offset += 4;

    //Lê a string do nome da estação, caso exista
    if (reg->tamNomeEstacao > 0) {
        reg->nomeEstacao = malloc((reg->tamNomeEstacao + 1) * sizeof(char));
        memcpy(reg->nomeEstacao, buffer + offset, reg->tamNomeEstacao);
        reg->nomeEstacao[reg->tamNomeEstacao] = '\0'; 
        offset += reg->tamNomeEstacao;
    }

    memcpy(&reg->tamNomeLinha, buffer + offset, 4);
    offset += 4;

    //Lê a string do nome da linha
    if (reg->tamNomeLinha > 0) {
        reg->nomeLinha = malloc((reg->tamNomeLinha + 1) * sizeof(char));
        memcpy(reg->nomeLinha, buffer + offset, reg->tamNomeLinha);
        reg->nomeLinha[reg->tamNomeLinha] = '\0'; //Finaliza com \0
        //O lixo que sobra até o byte 80 é ignorado 
    }
    return 1;
}


void escreve_registro_bin(FILE *bin, Registro *reg) {
    int bytes_escritos = 0;
    
    //Escreve os campos fixos do registro
    fwrite(&reg->removido, sizeof(char), 1, bin);         bytes_escritos += sizeof(char);
    fwrite(&reg->proximo, sizeof(int), 1, bin);           bytes_escritos += sizeof(int);
    fwrite(&reg->codEstacao, sizeof(int), 1, bin);        bytes_escritos += sizeof(int);
    fwrite(&reg->codLinha, sizeof(int), 1, bin);          bytes_escritos += sizeof(int);
    fwrite(&reg->codProxEstacao, sizeof(int), 1, bin);    bytes_escritos += sizeof(int);
    fwrite(&reg->distProxEstacao, sizeof(int), 1, bin);   bytes_escritos += sizeof(int);
    fwrite(&reg->codLinhaIntegra, sizeof(int), 1, bin);   bytes_escritos += sizeof(int);
    fwrite(&reg->codEstIntegra, sizeof(int), 1, bin);     bytes_escritos += sizeof(int);
    
    //Escreve campos de tamanho variável 
    fwrite(&reg->tamNomeEstacao, sizeof(int), 1, bin);    bytes_escritos += sizeof(int);
    if (reg->tamNomeEstacao > 0 && reg->nomeEstacao != NULL) {
        fwrite(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, bin);
        bytes_escritos += reg->tamNomeEstacao;
    }
    
    fwrite(&reg->tamNomeLinha, sizeof(int), 1, bin);      bytes_escritos += sizeof(int);
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
}


static int le_int_ou_nulo(void) {
    char buf[MAX_TAMANHO_STRING];
    //Lê a entrada como string primeiro (ints ou "NULO")
    scanf("%s", buf);
    
    // Verifica se a string digitada é "NULO"
    if (strcmp(buf, "NULO") == 0) return -1;
    
    // Caso contrário, converte a string numérica para um int
    return atoi(buf);
}
 
/* Lê um campo string da stdin (entre aspas / NULO) para o registro */
static void le_string_campo(int *tam, char **ptr) {
    char buf[MAX_TAMANHO_STRING];
    ScanQuoteString(buf);
    if (strlen(buf) == 0) {
        *tam = 0;
        *ptr = NULL;
    } else {
        *tam = (int)strlen(buf);
        *ptr = malloc((*tam + 1) * sizeof(char));
        strcpy(*ptr, buf);
    }
}
 
/* Lê os dados de um novo registro da entrada padrão */
void le_novo_registro(Registro *reg) {
    inicializa_registro(reg);
 
    reg->codEstacao      = le_int_ou_nulo();
    le_string_campo(&reg->tamNomeEstacao, &reg->nomeEstacao);
    reg->codLinha        = le_int_ou_nulo();
    le_string_campo(&reg->tamNomeLinha,   &reg->nomeLinha);
    reg->codProxEstacao  = le_int_ou_nulo();
    reg->distProxEstacao = le_int_ou_nulo();
    reg->codLinhaIntegra = le_int_ou_nulo();
    reg->codEstIntegra   = le_int_ou_nulo();
 
    reg->removido = '0';
    reg->proximo  = -1;
}
 
void reescreve_registro_atualizado(FILE *bin, long byte_offset, Registro *reg) {
    fseek(bin, byte_offset, SEEK_SET);
 
    int bytes_escritos = 0;
 
    // Escrita dos campos fixos 31 bytes no total
    fwrite(&reg->removido,          sizeof(char), 1, bin); bytes_escritos += 1;
    fwrite(&reg->proximo,           sizeof(int),  1, bin); bytes_escritos += 4;
    fwrite(&reg->codEstacao,        sizeof(int),  1, bin); bytes_escritos += 4;
    fwrite(&reg->codLinha,          sizeof(int),  1, bin); bytes_escritos += 4;
    fwrite(&reg->codProxEstacao,    sizeof(int),  1, bin); bytes_escritos += 4;
    fwrite(&reg->distProxEstacao,   sizeof(int),  1, bin); bytes_escritos += 4;
    fwrite(&reg->codLinhaIntegra,   sizeof(int),  1, bin); bytes_escritos += 4;
    fwrite(&reg->codEstIntegra,     sizeof(int),  1, bin); bytes_escritos += 4;
 
    // Nome Estação
    fwrite(&reg->tamNomeEstacao,    sizeof(int),  1, bin); bytes_escritos += 4;
    if (reg->tamNomeEstacao > 0 && reg->nomeEstacao != NULL) {
        fwrite(reg->nomeEstacao, sizeof(char), reg->tamNomeEstacao, bin);
        bytes_escritos += reg->tamNomeEstacao;
    }
 
    // Nome Linha
    fwrite(&reg->tamNomeLinha,      sizeof(int),  1, bin); bytes_escritos += 4;
    if (reg->tamNomeLinha > 0 && reg->nomeLinha != NULL) {
        fwrite(reg->nomeLinha, sizeof(char), reg->tamNomeLinha, bin);
        bytes_escritos += reg->tamNomeLinha;
    }
 
    char lixo = LIXO; // Constante '$'
    while (bytes_escritos < TAM_REGISTRO) {
        fwrite(&lixo, sizeof(char), 1, bin);
        bytes_escritos++;
    }
}

// Calcula quantos bytes o registro ocupa (sem o padding de lixo)
int tamanho_util_registro(const Registro *reg) {
    //tamanho dos campos
    return 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4
           + reg->tamNomeEstacao
           + reg->tamNomeLinha;
}
 