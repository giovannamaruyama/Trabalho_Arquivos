#include <stdio.h>
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

TipoCampo identifica_campo(const char *nome_campo) {
    if (strcmp(nome_campo, "codEstacao") == 0) return CAMPO_COD_ESTACAO;
    if (strcmp(nome_campo, "nomeEstacao") == 0) return CAMPO_NOME_ESTACAO;
    if (strcmp(nome_campo, "codLinha") == 0) return CAMPO_COD_LINHA;
    if (strcmp(nome_campo, "nomeLinha") == 0) return CAMPO_NOME_LINHA;
    if (strcmp(nome_campo, "codProxEstacao") == 0) return CAMPO_COD_PROX_ESTACAO;
    if (strcmp(nome_campo, "distProxEstacao") == 0) return CAMPO_DIST_PROX_ESTACAO;
    if (strcmp(nome_campo, "codLinhaIntegra") == 0) return CAMPO_COD_LINHA_INTEGRA;
    if (strcmp(nome_campo, "codEstIntegra") == 0) return CAMPO_COD_EST_INTEGRA;
    return CAMPO_INVALIDO;
}

int satisfaz_criterio(const Registro *reg, const CriteriodBusca *criterio) {
    switch (criterio->campo) {
        case CAMPO_COD_ESTACAO:
            return (criterio->nulo) ? (reg->codEstacao == -1) : (reg->codEstacao == criterio->valor_int);
        case CAMPO_NOME_ESTACAO:
            if (criterio->nulo) return (reg->tamNomeEstacao == 0 || reg->nomeEstacao == NULL);  //Verifica se o usuário busca um nome nulo
            if (reg->nomeEstacao == NULL) return 0; //Se o usuário quer um nome específico, mas o registro não tem nome
            return strcmp(reg->nomeEstacao, criterio->valor_str) == 0; //compara as strings 
        case CAMPO_COD_LINHA:
            return (criterio->nulo) ? (reg->codLinha == -1) : (reg->codLinha == criterio->valor_int);
        case CAMPO_NOME_LINHA:
            if (criterio->nulo) return (reg->tamNomeLinha == 0 || reg->nomeLinha == NULL);
            if (reg->nomeLinha == NULL) return 0;
            return strcmp(reg->nomeLinha, criterio->valor_str) == 0; //compara as strings
        case CAMPO_COD_PROX_ESTACAO:
            return (criterio->nulo) ? (reg->codProxEstacao == -1) : (reg->codProxEstacao == criterio->valor_int);
        case CAMPO_DIST_PROX_ESTACAO:
            return (criterio->nulo) ? (reg->distProxEstacao == -1) : (reg->distProxEstacao == criterio->valor_int);
        case CAMPO_COD_LINHA_INTEGRA:
            return (criterio->nulo) ? (reg->codLinhaIntegra == -1) : (reg->codLinhaIntegra == criterio->valor_int);
        case CAMPO_COD_EST_INTEGRA:
            return (criterio->nulo) ? (reg->codEstIntegra == -1) : (reg->codEstIntegra == criterio->valor_int);
        default:
            return 0;
    }
}

int satisfaz_todos_criterios(const Registro *reg, const ConjuntoCriterios *conjunto) {
    for (int i = 0; i < conjunto->num_criterios; i++) {
        if (!satisfaz_criterio(reg, &conjunto->criterios[i])) {
            return 0; //Falha em um critério
        }
    }
    return 1; //Satisfaz todos os critérios
}

int le_criterios(ConjuntoCriterios *conjunto) {
    int m_criterios;
    //Lê a quantidade de campos que serão usados como filtro na busca
    if (scanf("%d", &m_criterios) != 1) return -1; 
    
    conjunto->num_criterios = 0;
    for (int i = 0; i < m_criterios; i++) {
        char nome_campo[MAX_TAMANHO_STRING];
        char valor_str[MAX_TAMANHO_STRING];
        
        scanf("%s", nome_campo);
        
        //Converte a string do nome do campo para o tipo em número
        TipoCampo campo = identifica_campo(nome_campo);
        if (campo == CAMPO_INVALIDO) return -1;
        
        CriteriodBusca *crit = &conjunto->criterios[conjunto->num_criterios];
        crit->campo = campo;
        
        //Tratamento específico para strings
        if (campo == CAMPO_NOME_ESTACAO || campo == CAMPO_NOME_LINHA) {
            ScanQuoteString(valor_str); // Usa função auxiliar para tratar aspas
            if (strlen(valor_str) == 0) { 
                crit->nulo = 1;
                crit->valor_int = -1;
            } else {
                crit->nulo = 0;
                strcpy(crit->valor_str, valor_str);
            }
        } 
        //Tratamento para ints
        else {
            scanf("%s", valor_str);
            
            //Verifica se o valor buscado é explicitamente nulo
            if (strcmp(valor_str, "NULO") == 0) {
                crit->nulo = 1;
                crit->valor_int = -1;
            } else {
                crit->nulo = 0;
                crit->valor_int = atoi(valor_str); //Converte string em int
            }
        }
        conjunto->num_criterios++;
    }
    return 0;
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

void aplica_atualizacao(Registro *reg, const ConjuntoCriterios *atualizacao) {
    // Percorre cada campo que deve ser alterado no registro
    for (int i = 0; i < atualizacao->num_criterios; i++) {
        const CriteriodBusca *upd = &atualizacao->criterios[i];

        switch (upd->campo) {
            case CAMPO_COD_ESTACAO:
                // Atualiza código da estação
                reg->codEstacao = upd->nulo ? -1 : upd->valor_int;
                break;
            
            case CAMPO_NOME_ESTACAO:
                // Libera memória antiga e aloca nova
                free(reg->nomeEstacao);
                if (upd->nulo) {
                    reg->nomeEstacao    = NULL;
                    reg->tamNomeEstacao = 0;
                } else {
                    reg->tamNomeEstacao = (int)strlen(upd->valor_str);
                    reg->nomeEstacao    = malloc((reg->tamNomeEstacao + 1) * sizeof(char));
                    strcpy(reg->nomeEstacao, upd->valor_str);
                }
                break;
            
            case CAMPO_COD_LINHA:
                // Atualiza código da linha
                reg->codLinha = upd->nulo ? -1 : upd->valor_int;
                break;
            
            case CAMPO_NOME_LINHA:
                // Libera memória antiga e aloca nova
                free(reg->nomeLinha);
                if (upd->nulo) {
                    reg->nomeLinha    = NULL;
                    reg->tamNomeLinha = 0;
                } else {
                    reg->tamNomeLinha = (int)strlen(upd->valor_str);
                    reg->nomeLinha    = malloc((reg->tamNomeLinha + 1) * sizeof(char));
                    strcpy(reg->nomeLinha, upd->valor_str);
                }
                break;
            
            case CAMPO_COD_PROX_ESTACAO:
                // tualiza código da próxima estação
                reg->codProxEstacao = upd->nulo ? -1 : upd->valor_int;
                break;
            
            case CAMPO_DIST_PROX_ESTACAO:
                //Atualiza distância para próxima estação
                reg->distProxEstacao = upd->nulo ? -1 : upd->valor_int;
                break;
            
            case CAMPO_COD_LINHA_INTEGRA:
                //Atualiza código da linha de integração
                reg->codLinhaIntegra = upd->nulo ? -1 : upd->valor_int;
                break;
            
            case CAMPO_COD_EST_INTEGRA:
                //Atualiza código da estação de integração
                reg->codEstIntegra = upd->nulo ? -1 : upd->valor_int;
                break;
            
            default:
                break;
        }
    }
}
// Calcula quantos bytes o registro ocupa (sem o padding de lixo)
int tamanho_util_registro(const Registro *reg) {
    //tamanho dos campos
    return 1 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4
           + reg->tamNomeEstacao
           + reg->tamNomeLinha;
}
 