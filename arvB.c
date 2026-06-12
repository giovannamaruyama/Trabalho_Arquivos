//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "features.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int criar_arvoreB(char *nome_arquivo) {
    FILE *arv = fopen(nome_arquivo, "wb");
    if (arv == NULL) return 0; // erro ao abrir
    CabecalhoArvoreB cab;
    inicializa_cabecalho_arvoreB(&cab);
    escreve_cabecalho_arvoreB(arv, &cab);
    fflush(arv);
    fclose(arv);
    return 1; // sucesso
}

FILE* abrir_arvoreB(char *nome_arquivo, char *modo) {
    FILE *arv = fopen(nome_arquivo, modo);
    if (arv == NULL) return NULL; // falha na abertura
    fseek(arv, 0, SEEK_END);
    long tamanho = ftell(arv);
    rewind(arv);
    if (tamanho == 0) { // cria cabecalho se vazio
        CabecalhoArvoreB cab;
        inicializa_cabecalho_arvoreB(&cab);
        escreve_cabecalho_arvoreB(arv, &cab);
        fflush(arv);
    }
    return arv;
}

void fechar_arvoreB(FILE *arv, char *nome_arquivo) {
    if (arv == NULL) return;
    atualiza_status_arvoreB(arv, '1'); // salva como consistente
    fflush(arv);
    fclose(arv);
    BinarioNaTela(nome_arquivo);
}

void inicializa_cabecalho_arvoreB(CabecalhoArvoreB *cab) {
    cab->status = '0'; 
    cab->noRaiz = -1;  // sem raiz ainda
    cab->topo = -1;  
    cab->proxRRN = 0; 
    cab->nroNos = 0; 
}

CabecalhoArvoreB le_cabecalho_arvoreB(FILE *arv) {
    CabecalhoArvoreB cab;
    fseek(arv, 0, SEEK_SET);
    fread(&cab.status, sizeof(char), 1, arv);
    fread(&cab.noRaiz, sizeof(int), 1, arv);
    fread(&cab.topo, sizeof(int), 1, arv);
    fread(&cab.proxRRN, sizeof(int), 1, arv);
    fread(&cab.nroNos, sizeof(int), 1, arv);
    return cab; // retorna struct preenchida
}

void escreve_cabecalho_arvoreB(FILE *arv, CabecalhoArvoreB *cab) {
    fseek(arv, 0, SEEK_SET);
    fwrite(&cab->status, sizeof(char), 1, arv);
    fwrite(&cab->noRaiz, sizeof(int), 1, arv);
    fwrite(&cab->topo, sizeof(int), 1, arv);
    fwrite(&cab->proxRRN, sizeof(int), 1, arv);
    fwrite(&cab->nroNos, sizeof(int), 1, arv);
    fflush(arv);
}

void atualiza_status_arvoreB(FILE *arv, char status) {
    fseek(arv, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, arv); // atualiza o status byte 0
    fflush(arv);
}

void inicializa_no_arvoreB(NoArvoreB *no, int tipo) {
    no->removido = '0'; 
    no->proximo = -1;  
    no->tipoNo = tipo;
    no->nroChaves = 0; 
    for (int i = 0; i < 3; i++) { // limpa chaves e prs
        no->C[i] = -1;
        no->PR[i] = -1;
    }
    for (int i = 0; i < 4; i++) { // limpa filhos
        no->P[i] = -1;
    }
}

NoArvoreB le_no_arvoreB(FILE *arv, int rrn) {
    NoArvoreB no;
    long byte_offset = 17 + ((long)rrn * 53); // tamanho fixo do no
    fseek(arv, byte_offset, SEEK_SET);
    fread(&no.removido, sizeof(char), 1, arv);
    fread(&no.proximo, sizeof(int), 1, arv);
    fread(&no.tipoNo, sizeof(int), 1, arv);
    fread(&no.nroChaves, sizeof(int), 1, arv);
    for (int i = 0; i < 3; i++) {
        fread(&no.C[i], sizeof(int), 1, arv);
        fread(&no.PR[i], sizeof(int), 1, arv);
    }
    for (int i = 0; i < 4; i++) {
        fread(&no.P[i], sizeof(int), 1, arv);
    }
    return no;
}

int escreve_no_arvoreB(FILE *arv, int rrn, NoArvoreB *no) {
    CabecalhoArvoreB cab;
    if (rrn == -1) { // precisa pegar novo rrn
        cab = le_cabecalho_arvoreB(arv);
        rrn = cab.proxRRN;
        cab.proxRRN++;  
        escreve_cabecalho_arvoreB(arv, &cab); 
    }
    long byte_offset = 17 + ((long)rrn * 53);
    fseek(arv, byte_offset, SEEK_SET);
    fwrite(&no->removido, 1, 1, arv);
    fwrite(&no->proximo, 4, 1, arv);
    fwrite(&no->tipoNo, 4, 1, arv);
    fwrite(&no->nroChaves, 4, 1, arv);
    for (int i = 0; i < 3; i++) {
        fwrite(&no->C[i], 4, 1, arv);
        fwrite(&no->PR[i], 4, 1, arv);
    }
    for (int i = 0; i < 4; i++) {
        fwrite(&no->P[i], 4, 1, arv);
    }
    fflush(arv);
    return rrn; // devolve rrn onde gravou
}

int procura_posicao(NoArvoreB *no, int chave) {
    if (no == NULL || no->nroChaves < 0) return 0;
    for (int i = 0; i < no->nroChaves; i++) {  
        if (chave < no->C[i]) return i; // achou posicao correta
    }
    return no->nroChaves;
}

int busca_em_no(NoArvoreB *no, int chave, int *pr) {
    if (no == NULL || pr == NULL) return 0;
    for (int i = 0; i < no->nroChaves; i++) {
        if (no->C[i] == chave) {
            *pr = no->PR[i]; // guarda referencia
            return 1; 
        }
        if (chave < no->C[i]) return 0; // ordem crescente corta busca
    }
    return 0;
}

int inserir_recursivo(FILE *arv, int rrn_atual, int chave, int pr, int *promo_filho_dir, int *promo_chave, int *promo_pr) {
    // Condicao de parada: fundo falso abaixo da folha
    // manda a chave para cima
    if (rrn_atual == -1) {
        *promo_chave = chave;
        *promo_pr = pr;
        *promo_filho_dir = -1;
        return PROMOCAO;
    }

    NoArvoreB no = le_no_arvoreB(arv, rrn_atual);

    int dummy;
    if (busca_em_no(&no, chave, &dummy)) return ERRO; // chave ja existe

    int pos = procura_posicao(&no, chave);

    int p_b_rrn, p_b_chave, p_b_pr;
    int retorno = inserir_recursivo(arv, no.P[pos], chave, pr, &p_b_rrn, &p_b_chave, &p_b_pr);

    if (retorno == ERRO || retorno == SEM_PROMOCAO) return retorno;

    if (no.nroChaves < 3) {
        int pos_ins = procura_posicao(&no, p_b_chave);

        for (int i = no.nroChaves; i > pos_ins; i--) { // abre espaco para chaves
            no.C[i] = no.C[i-1];
            no.PR[i] = no.PR[i-1];
        }
        for (int i = no.nroChaves + 1; i > pos_ins + 1; i--) { // abre espaco para ponteiros
            no.P[i] = no.P[i-1];
        }

        no.C[pos_ins] = p_b_chave;
        no.PR[pos_ins] = p_b_pr;
        no.P[pos_ins+1] = p_b_rrn;
        no.nroChaves++;

        escreve_no_arvoreB(arv, rrn_atual, &no);
        return SEM_PROMOCAO; // inserido sem estourar
    } else {
        // no cheio: aplica split
        int c_temp[4], pr_temp[4], p_temp[5];
        for (int i = 0; i < 3; i++) { // joga dados pro vetor temporario
            c_temp[i] = no.C[i];
            pr_temp[i] = no.PR[i];
            p_temp[i] = no.P[i];
        }
        p_temp[3] = no.P[3];
        p_temp[4] = -1;

        int p_ins = 0;
        while (p_ins < 3 && p_b_chave > c_temp[p_ins]) p_ins++;

        for (int i = 3; i > p_ins; i--) {
            c_temp[i] = c_temp[i-1];
            pr_temp[i] = pr_temp[i-1];
        }
        for (int i = 4; i > p_ins + 1; i--) {
            p_temp[i] = p_temp[i-1];
        }

        c_temp[p_ins] = p_b_chave;
        pr_temp[p_ins] = p_b_pr;
        p_temp[p_ins+1] = p_b_rrn;

        *promo_chave = c_temp[2]; // define elemento promovido
        *promo_pr = pr_temp[2];

        no.nroChaves = 2; // esquerdo fica com duas chaves
        no.C[0] = c_temp[0]; no.PR[0] = pr_temp[0]; no.P[0] = p_temp[0];
        no.C[1] = c_temp[1]; no.PR[1] = pr_temp[1]; no.P[1] = p_temp[1];
        no.P[2] = p_temp[2];
        no.C[2] = -1; no.PR[2] = -1; no.P[3] = -1; 

        int tipo_metades = (no.P[0] == -1) ? -1 : 1;
        no.tipoNo = tipo_metades; 

        NoArvoreB novo_no;
        inicializa_no_arvoreB(&novo_no, tipo_metades);
        novo_no.nroChaves = 1; // direito fica com uma chave
        novo_no.C[0] = c_temp[3]; novo_no.PR[0] = pr_temp[3];
        novo_no.P[0] = p_temp[3]; novo_no.P[1] = p_temp[4];

        CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
        int rrn_novo;
        if (cab.topo != -1) { // prioritariamente busca na pilha
            rrn_novo = cab.topo;
            NoArvoreB removido = le_no_arvoreB(arv, rrn_novo);
            cab.topo = removido.proximo;
        } else {
            rrn_novo = cab.proxRRN;
            cab.proxRRN++;
        }
        cab.nroNos++;
        escreve_cabecalho_arvoreB(arv, &cab);

        *promo_filho_dir = rrn_novo;

        escreve_no_arvoreB(arv, rrn_atual, &no);
        escreve_no_arvoreB(arv, rrn_novo, &novo_no);

        return PROMOCAO; // avisa o pai sobre split
    }
}

int inserir_arvoreB(FILE *arv, int chave, int pr) {
    if (arv == NULL || pr < 0) return 0;

    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    atualiza_status_arvoreB(arv, '0'); // comeca a operar altera status

    if (cab.noRaiz == -1) {
        NoArvoreB raiz;
        inicializa_no_arvoreB(&raiz, -1); // Raiz inicial é folha = -1
        raiz.C[0] = chave;
        raiz.PR[0] = pr;
        raiz.nroChaves = 1;

        int rrn_raiz;
        if (cab.topo != -1) {
            rrn_raiz = cab.topo;
            NoArvoreB rem = le_no_arvoreB(arv, rrn_raiz);
            cab.topo = rem.proximo;
        } else {
            rrn_raiz = cab.proxRRN;
            cab.proxRRN++;
        }

        escreve_no_arvoreB(arv, rrn_raiz, &raiz);

        cab.noRaiz = rrn_raiz;
        cab.nroNos++;
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        return 1;
    }

    int promo_filho_dir, promo_chave, promo_pr;
    int retorno = inserir_recursivo(arv, cab.noRaiz, chave, pr, &promo_filho_dir, &promo_chave, &promo_pr);

    if (retorno == ERRO) { // limpa erro e sai
        cab = le_cabecalho_arvoreB(arv);
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        return 1; 
    }

    if (retorno == PROMOCAO) {
        NoArvoreB nova_raiz;
        inicializa_no_arvoreB(&nova_raiz, 0); // Nova raiz interna = 0
        nova_raiz.C[0] = promo_chave;
        nova_raiz.PR[0] = promo_pr;
        nova_raiz.P[0] = cab.noRaiz;
        nova_raiz.P[1] = promo_filho_dir;
        nova_raiz.nroChaves = 1;

        cab = le_cabecalho_arvoreB(arv);
        int rrn_nova_raiz;
        if (cab.topo != -1) {
            rrn_nova_raiz = cab.topo;
            NoArvoreB rem = le_no_arvoreB(arv, rrn_nova_raiz);
            cab.topo = rem.proximo;
        } else {
            rrn_nova_raiz = cab.proxRRN;
            cab.proxRRN++;
        }

        escreve_no_arvoreB(arv, rrn_nova_raiz, &nova_raiz);

        cab.noRaiz = rrn_nova_raiz;
        cab.nroNos++;
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        return 1; // nova raiz criada com sucesso
    }

    cab = le_cabecalho_arvoreB(arv);
    cab.status = '1'; // encerra como consistente
    escreve_cabecalho_arvoreB(arv, &cab);
    return 1;
}

// Passamos o byte offset no lugar do rrn
int construir_arvoreB(FILE *arv_dados, FILE *arv_indice) {
    if (arv_dados == NULL || arv_indice == NULL) return 0;
    
    fseek(arv_dados, 0, SEEK_SET);
    char status; int topo, proxRRN, nroEst, nroPares;
    fread(&status, sizeof(char), 1, arv_dados);
    fread(&topo, sizeof(int), 1, arv_dados);
    fread(&proxRRN, sizeof(int), 1, arv_dados);
    
    for (int rrn = 0; rrn < proxRRN; rrn++) {
        long byte_offset = 17 + ((long)rrn * 80); // calcula offset absoluto
        fseek(arv_dados, byte_offset, SEEK_SET);
        
        Registro reg;
        if (ler_registro_bin(arv_dados, &reg)) {
            if (reg.removido == '0' && reg.codEstacao != -1) {
                inserir_arvoreB(arv_indice, reg.codEstacao, (int)byte_offset);
            }
            libera_registro(&reg); // desaloca campos dinamicos
        }
    }
    return 1;
}

int buscar_arvoreB(FILE *arv, int chave, int *pr) {
    if (arv == NULL || pr == NULL) return 0;
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    if (cab.noRaiz == -1) return 0; // index sem chaves
    int rrn_atual = cab.noRaiz;

    while (rrn_atual != -1) {
        NoArvoreB no = le_no_arvoreB(arv, rrn_atual);
        if (no.nroChaves < 0 || no.nroChaves > 3) return 0; // no corrompido
        if (busca_em_no(&no, chave, pr)) return 1; // encontrou
        int pos = procura_posicao(&no, chave);
        rrn_atual = no.P[pos]; // caminha pro proximo filho
    }
    return 0; // nao localizado
}