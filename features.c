#include "features.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_CAMPOS_BUSCA 8      // Máximo de critérios por busca
#define MAX_TAMANHO_STRING 256  // Máximo para string de busca

//Funcoes auxiliares 
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

void inicializa_cabecalho(Cabecalho *cab) {
    cab->status = '0'; 
    cab->topo = -1;
    cab->proxRRN = 0;
    cab->nroEstacoes = 0;
    cab->nroParesEstacao = 0;
}

void escreve_cabecalho(FILE *bin, Cabecalho *cab) {
    fseek(bin, 0, SEEK_SET);
    fwrite(&cab->status, sizeof(char), 1, bin);
    fwrite(&cab->topo, sizeof(int), 1, bin);
    fwrite(&cab->proxRRN, sizeof(int), 1, bin);
    fwrite(&cab->nroEstacoes, sizeof(int), 1, bin);
    fwrite(&cab->nroParesEstacao, sizeof(int), 1, bin);
}

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

//Função auxiliar para imprimir o registro 
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


//Funcoes auxiliares para ler o CSV:

//Verifica se um campo extraído do CSV eh nulo, vazio ou contém apenas espaços/quebras de linha.
int nulo_csv(char *campo) {
    if (campo == NULL) return 1;
    while (*campo == ' ' || *campo == '\t' || *campo == '\n' || *campo == '\r') {
        campo++;
    }
    return *campo == '\0';
}

//converte o char em int
int converte_csv(char *campo) {
    if (nulo_csv(campo)) {
        return NULO;
    }
    return atoi(campo);
}

// Função extrair campos do CSV 
char* extrai_campo_csv(char **campo_string) {
    if (*campo_string == NULL) return NULL;
    char *inicio = *campo_string;
    char *p = strchr(inicio, ','); 
    if (p != NULL) {
        *p = '\0';       
        *campo_string = p + 1; 
    } else {
        *campo_string = NULL;  
    }
    return inicio;
}

int ler_linha_csv(FILE *csv, Registro *reg){
    char linha[1024];

    if (fgets(linha, sizeof(linha), csv) == NULL) return 0; 
    linha[strcspn(linha, "\r\n")] = '\0';

    inicializa_registro(reg);
    char *ptr = linha;
    char *campo;
  
    campo = extrai_campo_csv(&ptr);
    reg->codEstacao = converte_csv(campo);
   
    campo = extrai_campo_csv(&ptr);
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

//Listas para o header:

typedef struct NoEstacao {
    char *nome;
    struct NoEstacao *prox;
} NoEstacao;

typedef struct NoDupla {
    int cod1;
    int cod2;
    struct NoDupla *prox;
} NoDupla;

void inserir_estacao(NoEstacao **lista, char *nome_estacao, int *contador_estacoes) {
    if (nome_estacao == NULL || strlen(nome_estacao) == 0) return;

    NoEstacao *atual = *lista;
    while (atual != NULL) {
        if (strcmp(atual->nome, nome_estacao) == 0) return; //confeere se ja nn existe uma estacao com o mesmo nome
        atual = atual->prox;
    }
    
    NoEstacao *novo = malloc(sizeof(NoEstacao)); //vai montando a lista de estacoes
    novo->nome = malloc((strlen(nome_estacao) + 1) * sizeof(char));
    strcpy(novo->nome, nome_estacao);
    novo->prox = *lista;
    *lista = novo;
    
    (*contador_estacoes)++; 
}

void inserir_par(NoDupla **lista, int cod1, int cod2, int *contador_pares) {
    if (cod1 == -1 || cod2 == -1) return;

    NoDupla *atual = *lista;
    while (atual != NULL) {
        if (atual->cod1 == cod1 && atual->cod2 == cod2) return; //confeere se ja nn existe uma dupla de estacoes igual
        atual = atual->prox;
    }
    
    NoDupla *novo_par = malloc(sizeof(NoDupla)); //monta a lista 
    novo_par->cod1 = cod1;
    novo_par->cod2 = cod2;
    novo_par->prox = *lista;
    *lista = novo_par;
    
    (*contador_pares)++; 
}

void liberar_lista_estacoes(NoEstacao *lista) {
    NoEstacao *atual = lista;
    while (atual != NULL) {
        NoEstacao *aux = atual;
        atual = atual->prox;
        free(aux->nome);
        free(aux);
    }
}

void liberar_lista_pares(NoDupla *lista) {
    NoDupla *atual = lista;
    while (atual != NULL) {
        NoDupla *aux = atual;
        atual = atual->prox;
        free(aux);
    }
}

//Funcionalidades:

void funcionalidade_1(char *nome_csv, char *nome_bin) {
    FILE *csv = fopen(nome_csv, "r"); 
    if (csv == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    FILE *bin = fopen(nome_bin, "wb");  
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(csv);
        return;
    }

    Cabecalho cab;
    inicializa_cabecalho(&cab);
    escreve_cabecalho(bin, &cab); 

    char buffer_desc[1024];
    if (fgets(buffer_desc, sizeof(buffer_desc), csv) == NULL) {
        fclose(csv);
        fclose(bin);
        return; 
    }

    NoEstacao *lista_estacoes = NULL;
    NoDupla *lista_pares = NULL;

    Registro reg;
    while (ler_linha_csv(csv, &reg)) {
        escreve_registro_bin(bin, &reg);
        cab.proxRRN++; 

        if (reg.tamNomeEstacao > 0) {
            inserir_estacao(&lista_estacoes, reg.nomeEstacao, &cab.nroEstacoes);
        }
        inserir_par(&lista_pares, reg.codEstacao, reg.codProxEstacao, &cab.nroParesEstacao);

        libera_registro(&reg);
    }

    liberar_lista_estacoes(lista_estacoes);
    liberar_lista_pares(lista_pares);

    cab.status = '1'; 
    escreve_cabecalho(bin, &cab);

    fclose(csv);
    fclose(bin);

    BinarioNaTela(nome_bin);
}

void funcionalidade_2(char *nome_bin) {
    FILE *bin = fopen(nome_bin, "rb"); //Abre para leitura binária
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cab;
    //Lê o status do arquivo 
    if (fread(&cab.status, sizeof(char), 1, bin) != 1 || cab.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }

    //Pula o resto do cabeçalho para posicionar a leitura no byte 17 (início dos registros)
    fseek(bin, 17, SEEK_SET);

    Registro reg;
    int registros_impressos = 0;

    //Loop lendo todos os registros de 80 em 80 bytes
    while (ler_registro_bin(bin, &reg)) {
        // Se não estiver logicamente removido, imprime
        if (reg.removido == '0') {
            imprime_registro(&reg);
            registros_impressos++;
        }
        libera_registro(&reg); 
    }

    //Caso o arquivo não possua nenhum registro válido 
    if (registros_impressos == 0) {
        printf("Registro inexistente.\n");
    }

    fclose(bin);
}

//Estrutura e funcoes auxiliares ter criterios de busca
typedef enum{
    CAMPO_COD_ESTACAO,
    CAMPO_NOME_ESTACAO,
    CAMPO_COD_LINHA,
    CAMPO_NOME_LINHA,
    CAMPO_COD_PROX_ESTACAO,
    CAMPO_DIST_PROX_ESTACAO,
    CAMPO_COD_LINHA_INTEGRA,
    CAMPO_COD_EST_INTEGRA,
    CAMPO_INVALIDO //default
} TipoCampo;

typedef struct {
    TipoCampo campo;
    int valor_int; //o int que deve ser buscado
    char valor_str[MAX_TAMANHO_STRING]; //o char que deve ser buscado
    int nulo;  //flag caso ele queira um valor NULO (1)
} CriteriodBusca;

typedef struct { //caso tenha mais que um criterio
    CriteriodBusca criterios[MAX_CAMPOS_BUSCA];
    int num_criterios;
} ConjuntoCriterios;

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
    if (scanf("%d", &m_criterios) != 1) return -1; 
    conjunto->num_criterios = 0;
    for (int i = 0; i < m_criterios; i++) {
        char nome_campo[MAX_TAMANHO_STRING];
        char valor_str[MAX_TAMANHO_STRING];
        
        scanf("%s", nome_campo);
        
        TipoCampo campo = identifica_campo(nome_campo);
        if (campo == CAMPO_INVALIDO) return -1;
        
        CriteriodBusca *crit = &conjunto->criterios[conjunto->num_criterios];
        crit->campo = campo;
        
        if (campo == CAMPO_NOME_ESTACAO || campo == CAMPO_NOME_LINHA) {
            ScanQuoteString(valor_str); 
            
            if (strlen(valor_str) == 0) { 
                crit->nulo = 1;
                crit->valor_int = -1;
            } else {
                crit->nulo = 0;
                strcpy(crit->valor_str, valor_str);
            }
        } else {
            scanf("%s", valor_str);
            
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

void funcionalidade_3(char *nome_bin, int num_buscas) {
    FILE *bin = fopen(nome_bin, "rb");
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    Cabecalho cab;
    if (fread(&cab.status, sizeof(char), 1, bin) != 1 || cab.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
    
    //Executa as N buscas
    for (int busca_num = 0; busca_num < num_buscas; busca_num++) {
        ConjuntoCriterios conjunto;
        if (le_criterios(&conjunto) != 0) break; 
        // Posiciona no início dos registros (byte 17) para varrer do começo
        fseek(bin, 17, SEEK_SET);
        Registro reg;
        int encontrados = 0;
        
        while (ler_registro_bin(bin, &reg)) {
            if (reg.removido == '0') {
                if (satisfaz_todos_criterios(&reg, &conjunto)) {
                    imprime_registro(&reg);
                    encontrados++;
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


void remove_logicamente(FILE *bin, Cabecalho *cab, int rrn_atual) {
    char removido = '1';
    int proximo_rrn = cab->topo; 
    
    long byte_offset_atual = 17 + ((long)rrn_atual * 80); 
    fseek(bin, byte_offset_atual, SEEK_SET);
    
    fwrite(&removido, sizeof(char), 1, bin);
    fwrite(&proximo_rrn, sizeof(int), 1, bin);
    
    cab->topo = rrn_atual; 
}


void funcionalidade_4(char *nome_bin, int num_remocoes) {
    FILE *bin = fopen(nome_bin, "rb+"); // rb+ é obrigatório para não apagar o ficheiro
    if (bin == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Cabecalho cab;
    // Leitura campo a campo para evitar problemas de alinhamento (padding)
    if (fread(&cab.status, sizeof(char), 1, bin) != 1 || cab.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        return;
    }
    fread(&cab.topo,            sizeof(int), 1, bin);
    fread(&cab.proxRRN,         sizeof(int), 1, bin);
    fread(&cab.nroEstacoes,     sizeof(int), 1, bin);
    fread(&cab.nroParesEstacao, sizeof(int), 1, bin);

    // Início da operação: Status = '0'
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

        // ler_registro_bin deve retornar 0 apenas no fim do ficheiro
        while (ler_registro_bin(bin, &reg)) {
            if (reg.removido == '0') {
                if (satisfaz_todos_criterios(&reg, &conjunto)) {
                    remove_logicamente(bin, &cab, rrn_contador);
                    cab.nroParesEstacao--;
                    // IMPORTANTE: Após o fwrite de 5 bytes, o ponteiro está "torto".
                    // Precisamos de o alinhar para o início do PRÓXIMO registo.
                    fseek(bin, TAM_CABECALHO + ((rrn_contador + 1) * TAM_REGISTRO), SEEK_SET);
                }
            }
            libera_registro(&reg);
            rrn_contador++;
        }
    }
  
    
    cab.status = '0'; //ATENÇÃO
    escreve_cabecalho(bin, &cab); 
    fflush(bin);

    fclose(bin);
    BinarioNaTela(nome_bin);
}