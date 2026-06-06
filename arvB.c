
#include "features.h"
#include "arvoreB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
//Cria um novo arquivo de índice Árvore-B vazio

int criar_arvoreB(char *nome_arquivo) {
    //abre arquivo em modo escrita binária
    FILE *arv = fopen(nome_arquivo, "wb");
    if (arv == NULL) {
        return 0;
    }
    
    //Inicializa cabeçalho com valores padrão
    CabecalhoArvoreB cab;
    inicializa_cabecalho_arvoreB(&cab);
    
    //escreve cabeçalho no início do arquivo
    escreve_cabecalho_arvoreB(arv, &cab);
    fflush(arv);
    
    //fecha o arquivo
    fclose(arv);
    return 1;
}
 
//abre um arquivo de índice Árvore-B existente
FILE* abrir_arvoreB(char *nome_arquivo, char *modo) {
    //Tenta abrir arquivo
    FILE *arv = fopen(nome_arquivo, modo);
    
    //retorna NULL se falhar
    if (arv == NULL) {
        printf("Falha no processamento do arquivo\n");
        return NULL;
    }
    
    //VERIFICA SE ARQUIVO TEM TAMANHO MÍNIMO (PELO MENOS CABEÇALHO)
    fseek(arv, 0, SEEK_END);
    long tamanho = ftell(arv);
    rewind(arv);
    
    //Se arquivo vazio, escreve cabeçalho
    if (tamanho == 0) {
        CabecalhoArvoreB cab;
        inicializa_cabecalho_arvoreB(&cab);
        escreve_cabecalho_arvoreB(arv, &cab);
        fflush(arv);
    }
    
    return arv;
}
 
//fecha arquivo Árvore-B e atualiza status para 1 (consistente)
void fechar_arvoreB(FILE *arv, char *nome_arquivo) {
    if (arv == NULL) return;
    //atualiza status para '1' (consistente)
    atualiza_status_arvoreB(arv, '1');
    fflush(arv);

    //fecha o arquivo
    fclose(arv);
    //exibe conteúdo
    BinarioNaTela(nome_arquivo);
}

//Inicializa cabeçalho com valores padrão (árvore vazia)
void inicializa_cabecalho_arvoreB(CabecalhoArvoreB *cab) {
    cab->status = '0';      //arquivo inconsistente no início
    cab->noRaiz = -1;       //arvore vazia
    cab->topo = -1;         //sem nós removidos
    cab->proxRRN = 0;       //próximo RRN a usar
    cab->nroNos = 0;        //Quantidade de nós = 0
}
 
//Lê cabeçalho do arquivo (posição 0, 17 bytes)
CabecalhoArvoreB le_cabecalho_arvoreB(FILE *arv) {
    CabecalhoArvoreB cab;
    
    //posiciona no início do arquivo
    fseek(arv, 0, SEEK_SET);
    
    //lê campo a campo para evitar padding
    fread(&cab.status,      sizeof(char), 1, arv);
    fread(&cab.noRaiz,      sizeof(int),  1, arv);
    fread(&cab.topo,        sizeof(int),  1, arv);
    fread(&cab.proxRRN,     sizeof(int),  1, arv);
    fread(&cab.nroNos,      sizeof(int),  1, arv);
    
    //retorna o cabeçalho preenchido
    return cab;
}
 
//escreve cabeçalho no arquivo (posição 0, 17 bytes)
void escreve_cabecalho_arvoreB(FILE *arv, CabecalhoArvoreB *cab) {
    //posiciona no início do arquivo
    fseek(arv, 0, SEEK_SET);
    
    //escreve campo a campo para evitar padding
    fwrite(&cab->status,sizeof(char), 1, arv);
    fwrite(&cab->noRaiz, sizeof(int),  1, arv);
    fwrite(&cab->topo,sizeof(int),  1, arv);
    fwrite(&cab->proxRRN,sizeof(int),  1, arv);
    fwrite(&cab->nroNos, sizeof(int),  1, arv);
}
 
//somente atualiza o status do cabeçalho (marcando consistência)
void atualiza_status_arvoreB(FILE *arv, char status) {
    //posiciona no primeiro byte (STATUS)
    fseek(arv, 0, SEEK_SET);
    //escreve novo status
    fwrite(&status, sizeof(char), 1, arv);
}
 
//Inicializa nó com valores padrão
void inicializa_no_arvoreB(NoArvoreB *no, int tipo) {
    no->removido = '0';     //não está removido
    no->proximo = -1;       //sem próximo na pilha
    no->tipoNo = tipo;      //tipo definido
    no->nroChaves = 0;      //começa sem chaves
    
    //inicializa chaves com -1 (ausente)
    for (int i = 0; i < MAX_CHAVES; i++) {
        no->C[i] = -1;
        no->PR[i] = -1;
    }
    
    //inicializa ponteiros com -1 (ausente)
    for (int i = 0; i < MAX_FILHOS; i++) {
        no->P[i] = -1;
    }
}
 
//Lê nó do arquivo pelo RRN
NoArvoreB le_no_arvoreB(FILE *arv, int rrn) {
    NoArvoreB no;
    //calcula posição do nó no arquivo
    long byte_offset = TAM_CABECALHO_ARVORE_B + ((long)rrn * TAM_NO_ARVORE_B);
    
    //posiciona no nó
    fseek(arv, byte_offset, SEEK_SET);
    
    //lê campo a campo para evitar padding (53 bytes total)
    fread(&no.removido, sizeof(char), 1, arv);
    fread(&no.proximo,sizeof(int),  1, arv);
    fread(&no.tipoNo, sizeof(int),  1, arv);
    fread(&no.nroChaves,sizeof(int),  1, arv);
    
    //le chaves e referencia
    for (int i = 0; i < MAX_CHAVES; i++) {
        fread(&no.C[i],     sizeof(int),  1, arv);
        fread(&no.PR[i],    sizeof(int),  1, arv);
    }
    
    //lê ponteiros para filhos
    for (int i = 0; i < MAX_FILHOS; i++) {
        fread(&no.P[i],     sizeof(int),  1, arv);
    }
    
    return no;
}
 
//escreve nó no arquivo na posição RRN
int escreve_no_arvoreB(FILE *arv, int rrn, NoArvoreB *no) {
    //se rrn == -1, usar próximo disponível
    if (rrn == -1) {
        CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
        rrn = cab.proxRRN;
    }
    
    //calcula posição do nó
    long byte_offset = TAM_CABECALHO_ARVORE_B + ((long)rrn * TAM_NO_ARVORE_B);
    
    //Posiciona no nó
    fseek(arv, byte_offset, SEEK_SET);
    
    //escreve campo a campo (53 bytes total)
    fwrite(&no->removido,   sizeof(char), 1, arv);
    fwrite(&no->proximo,    sizeof(int),  1, arv);
    fwrite(&no->tipoNo,     sizeof(int),  1, arv);
    fwrite(&no->nroChaves,  sizeof(int),  1, arv);
    
    //escreve chaves e referências
    for (int i = 0; i < MAX_CHAVES; i++) {
        fwrite(&no->C[i],   sizeof(int),  1, arv);
        fwrite(&no->PR[i],  sizeof(int),  1, arv);
    }
    
    //Escreve ponteiros para filhos
    for (int i = 0; i < MAX_FILHOS; i++) {
        fwrite(&no->P[i],   sizeof(int),  1, arv);
    }
    
    return rrn;
}
 
//procura posição para inserir chave em nó ordenado
int procura_posicao(NoArvoreB *no, int chave) {
    //percorre chaves do nó em ordem
    for (int i = 0; i < no->nroChaves; i++) {
        if (chave < no->C[i]) {
            return i;
        }
    }
    return no->nroChaves;
}
 
//Insere chave em nó não cheio
void insere_em_no(NoArvoreB *no, int chave, int pr) {
    //encontra posição correta
    int pos = procura_posicao(no, chave);
    
    //desloca chaves para a direita
    for (int i = no->nroChaves; i > pos; i--) {
        no->C[i] = no->C[i-1];
        no->PR[i] = no->PR[i-1];
    }
    
    //insere nova chave
    no->C[pos] = chave;
    no->PR[pos] = pr;
    no->nroChaves++;
}
 

 

 //Imprime informações de um nó (debug)
void imprime_no_arvoreB(NoArvoreB *no, int rrn) {
    printf("=== Nó RRN %d ===\n", rrn);
    printf("Removido: %c\n", no->removido);
    printf("Tipo: %d\n", no->tipoNo);
    printf("Chaves: %d\n", no->nroChaves);
    for (int i = 0; i < no->nroChaves; i++) {
        printf("  C[%d]=%d PR[%d]=%d\n", i, no->C[i], i, no->PR[i]);
    }
    printf("Filhos: ");
    for (int i = 0; i <= no->nroChaves; i++) {
        printf("P[%d]=%d ", i, no->P[i]);
    }
    printf("\n");
}
 
//imprime cabeçalho da Árvore-B 
void imprime_cabecalho_arvoreB(CabecalhoArvoreB *cab) {
    printf("=== Cabeçalho Árvore-B ===\n");
    printf("Status: %c\n", cab->status);
    printf("Raiz: %d\n", cab->noRaiz);
    printf("Topo (removidos): %d\n", cab->topo);
    printf("Próximo RRN: %d\n", cab->proxRRN);
    printf("Total de nós: %d\n", cab->nroNos);
}