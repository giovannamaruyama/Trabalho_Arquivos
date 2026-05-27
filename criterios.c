#include "features.h"
#include <string.h>


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

/* ═══════════════════════════════════════════════════════
 * VERIFICAÇÃO DE CRITÉRIOS
 * ═══════════════════════════════════════════════════════ */

int satisfaz_criterio(const Registro *reg, const CriteriodBusca *criterio) {
    switch (criterio->campo) {
        case CAMPO_COD_ESTACAO:
            return (criterio->nulo) ? (reg->codEstacao == -1) 
                                    : (reg->codEstacao == criterio->valor_int);
        
        case CAMPO_NOME_ESTACAO:
            if (criterio->nulo) 
                return (reg->tamNomeEstacao == 0 || reg->nomeEstacao == NULL);
            if (reg->nomeEstacao == NULL) 
                return 0;
            return strcmp(reg->nomeEstacao, criterio->valor_str) == 0;
        
        case CAMPO_COD_LINHA:
            return (criterio->nulo) ? (reg->codLinha == -1) 
                                    : (reg->codLinha == criterio->valor_int);
        
        case CAMPO_NOME_LINHA:
            if (criterio->nulo) 
                return (reg->tamNomeLinha == 0 || reg->nomeLinha == NULL);
            if (reg->nomeLinha == NULL) 
                return 0;
            return strcmp(reg->nomeLinha, criterio->valor_str) == 0;
        
        case CAMPO_COD_PROX_ESTACAO:
            return (criterio->nulo) ? (reg->codProxEstacao == -1) 
                                    : (reg->codProxEstacao == criterio->valor_int);
        
        case CAMPO_DIST_PROX_ESTACAO:
            return (criterio->nulo) ? (reg->distProxEstacao == -1) 
                                    : (reg->distProxEstacao == criterio->valor_int);
        
        case CAMPO_COD_LINHA_INTEGRA:
            return (criterio->nulo) ? (reg->codLinhaIntegra == -1) 
                                    : (reg->codLinhaIntegra == criterio->valor_int);
        
        case CAMPO_COD_EST_INTEGRA:
            return (criterio->nulo) ? (reg->codEstIntegra == -1) 
                                    : (reg->codEstIntegra == criterio->valor_int);
        
        default:
            return 0;
    }
}

int satisfaz_todos_criterios(const Registro *reg, const ConjuntoCriterios *conjunto) {
    for (int i = 0; i < conjunto->num_criterios; i++) {
        if (!satisfaz_criterio(reg, &conjunto->criterios[i])) {
            return 0;
        }
    }
    return 1;
}

int busca_por_cod_estacao_unico(const ConjuntoCriterios *conjunto) {
    return (conjunto->num_criterios == 1 && 
            conjunto->criterios[0].campo == CAMPO_COD_ESTACAO &&
            !conjunto->criterios[0].nulo);
}


int le_criterios(ConjuntoCriterios *conjunto) {
    int m_criterios;
    if (scanf("%d", &m_criterios) != 1) return -1;
    
    conjunto->num_criterios = 0;
    
    for (int i = 0; i < m_criterios; i++) {
        char nome_campo[MAX_TAMANHO_STRING];
        char valor_str[MAX_TAMANHO_STRING];
        
        if (scanf("%s", nome_campo) != 1) return -1;
        
        TipoCampo campo = identifica_campo(nome_campo);
        if (campo == CAMPO_INVALIDO) return -1;
        
        CriteriodBusca *crit = &conjunto->criterios[conjunto->num_criterios];
        crit->campo = campo;
        
        // Tratamento para campos string
        if (campo == CAMPO_NOME_ESTACAO || campo == CAMPO_NOME_LINHA) {
            ScanQuoteString(valor_str);
            if (strlen(valor_str) == 0) {
                crit->nulo = 1;
                crit->valor_int = -1;
            } else {
                crit->nulo = 0;
                strcpy(crit->valor_str, valor_str);
            }
        } 
        // Tratamento para campos inteiros
        else {
            if (scanf("%s", valor_str) != 1) return -1;
            
            if (strcmp(valor_str, "NULO") == 0) {
                crit->nulo = 1;
                crit->valor_int = -1;
            } else {
                crit->nulo = 0;
                crit->valor_int = atoi(valor_str);
            }
        }
        
        conjunto->num_criterios++;
    }
    return 0;
}

//le e aplica as atualizacoes

int le_atualizacoes(ConjuntoAtualizacoes *conjunto) {
    int p;
    if (scanf("%d", &p) != 1) return -1;
    
    conjunto->num_atualizacoes = 0;
    
    for (int i = 0; i < p; i++) {
        char nome_campo[MAX_TAMANHO_STRING];
        char valor_str[MAX_TAMANHO_STRING];
        
        if (scanf("%s", nome_campo) != 1) return -1;
        
        TipoCampo campo = identifica_campo(nome_campo);
        if (campo == CAMPO_INVALIDO) return -1;
        
        CriteriodBusca *atu = &conjunto->atualizacoes[conjunto->num_atualizacoes];
        atu->campo = campo;
        
        if (campo == CAMPO_NOME_ESTACAO || campo == CAMPO_NOME_LINHA) {
            ScanQuoteString(valor_str);
            if (strlen(valor_str) == 0) {
                atu->nulo = 1;
                atu->valor_int = -1;
                atu->valor_str[0] = '\0';
            } else {
                atu->nulo = 0;
                strcpy(atu->valor_str, valor_str);
            }
        } 
        else {
            if (scanf("%s", valor_str) != 1) return -1;
            
            if (strcmp(valor_str, "NULO") == 0) {
                atu->nulo = 1;
                atu->valor_int = -1;
            } else {
                atu->nulo = 0;
                atu->valor_int = atoi(valor_str);
            }
        }
        
        conjunto->num_atualizacoes++;
    }
    return 0;
}

void aplica_atualizacao(Registro *reg, const ConjuntoAtualizacoes *atualizacao) {
    for (int i = 0; i < atualizacao->num_atualizacoes; i++) {
        const CriteriodBusca *atu = &atualizacao->atualizacoes[i];

        switch (atu->campo) {
            case CAMPO_COD_ESTACAO:
                reg->codEstacao = atu->nulo ? -1 : atu->valor_int;
                break;

            case CAMPO_NOME_ESTACAO:
                free(reg->nomeEstacao);
                if (atu->nulo) {
                    reg->nomeEstacao = NULL;
                    reg->tamNomeEstacao = 0;
                } else {
                    int tam = (int)strlen(atu->valor_str);
                    reg->nomeEstacao = (char *)malloc((tam + 1) * sizeof(char));
                    if (reg->nomeEstacao != NULL) {
                        strcpy(reg->nomeEstacao, atu->valor_str);
                        reg->tamNomeEstacao = tam;
                    }
                }
                break;

            case CAMPO_COD_LINHA:
                reg->codLinha = atu->nulo ? -1 : atu->valor_int;
                break;

            case CAMPO_NOME_LINHA:
                free(reg->nomeLinha);
                if (atu->nulo) {
                    reg->nomeLinha = NULL;
                    reg->tamNomeLinha = 0;
                } else {
                    int tam = (int)strlen(atu->valor_str);
                    reg->nomeLinha = (char *)malloc((tam + 1) * sizeof(char));
                    if (reg->nomeLinha != NULL) {
                        strcpy(reg->nomeLinha, atu->valor_str);
                        reg->tamNomeLinha = tam;
                    }
                }
                break;

            case CAMPO_COD_PROX_ESTACAO:
                reg->codProxEstacao = atu->nulo ? -1 : atu->valor_int;
                break;

            case CAMPO_DIST_PROX_ESTACAO:
                reg->distProxEstacao = atu->nulo ? -1 : atu->valor_int;
                break;

            case CAMPO_COD_LINHA_INTEGRA:
                reg->codLinhaIntegra = atu->nulo ? -1 : atu->valor_int;
                break;

            case CAMPO_COD_EST_INTEGRA:
                reg->codEstIntegra = atu->nulo ? -1 : atu->valor_int;
                break;

            default:
                break;
        }
    }
}

//Remocao logica
void remove_logicamente(FILE *bin, Cabecalho *cab, int rrn_atual) {
    char removido = '1';
    int proximo_rrn = cab->topo;
    
    long byte_offset_atual = TAM_CABECALHO + ((long)rrn_atual * TAM_REGISTRO);
    fseek(bin, byte_offset_atual, SEEK_SET);
    
    fwrite(&removido, sizeof(char), 1, bin);
    fwrite(&proximo_rrn, sizeof(int), 1, bin);
    
    cab->topo = rrn_atual;
}