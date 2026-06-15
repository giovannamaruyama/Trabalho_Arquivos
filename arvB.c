//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "feature.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int criar_arvoreB(char *nome_arquivo) {
    FILE *arv = fopen(nome_arquivo, "wb");
    if (arv == NULL) return 0; //confere se o arquivo existe
    CabecalhoArvoreB cab;
    inicializa_cabecalho_arvoreB(&cab);
    escreve_cabecalho_arvoreB(arv, &cab);
    fflush(arv);
    fclose(arv);
    return 1;
}

FILE* abrir_arvoreB(char *nome_arquivo, char *modo) {
    FILE *arv = fopen(nome_arquivo, modo);
    if (arv == NULL) return NULL;
    fseek(arv, 0, SEEK_END);
    long tamanho = ftell(arv);
    rewind(arv);
    if (tamanho == 0) { // cria o cabecalho do zero se o arquivo sumiu
        CabecalhoArvoreB cab;
        inicializa_cabecalho_arvoreB(&cab);
        escreve_cabecalho_arvoreB(arv, &cab);
        fflush(arv);
    }
    return arv;
}

void fechar_arvoreB(FILE *arv, char *nome_arquivo) {
    if (arv == NULL) return;
    atualiza_status_arvoreB(arv, '1'); // fecha como consistente
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
    return cab;
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
    fwrite(&status, sizeof(char), 1, arv); // muda o byte de status
    fflush(arv);
}

void inicializa_no_arvoreB(NoArvoreB *no, int tipo) {
    no->removido = '0'; 
    no->proximo = -1;  
    no->tipoNo = tipo;
    no->nroChaves = 0; 
    for (int i = 0; i < 3; i++) { // zera tudo
        no->C[i] = -1;
        no->PR[i] = -1;
    }
    for (int i = 0; i < 4; i++) { // limpa ponteiros de filhos
        no->P[i] = -1;
    }
}

NoArvoreB le_no_arvoreB(FILE *arv, int rrn) {
    NoArvoreB no;
    long byte_offset = 17 + ((long)rrn * 53); // pula o cabecalho fixo
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
    if (rrn == -1) { // pega rrn novo se precisar
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
    return rrn;
}

int procura_posicao(NoArvoreB *no, int chave) {
    if (no == NULL || no->nroChaves < 0) return 0;
    for (int i = 0; i < no->nroChaves; i++) {  
        if (chave < no->C[i]) return i; // acha onde a chave deve entrar
    }
    return no->nroChaves;
}

int busca_em_no(NoArvoreB *no, int chave, int *pr) {
    if (no == NULL || pr == NULL) return 0;
    for (int i = 0; i < no->nroChaves; i++) {
        if (no->C[i] == chave) {
            *pr = no->PR[i]; // achou a chave no no
            return 1; 
        }
        if (chave < no->C[i]) return 0; // corta o laco mais cedo
    }
    return 0;
}

int inserir_recursivo(FILE *arv, int rrn_atual, int chave, int pr, int *promo_filho_dir, int *promo_chave, int *promo_pr) {
    if (rrn_atual == -1) { // chegou no fundo, avisa pra subir
        *promo_chave = chave;
        *promo_pr = pr;
        *promo_filho_dir = -1;
        return PROMOCAO; // manda sinal de promocao pro pai
    }

    NoArvoreB no = le_no_arvoreB(arv, rrn_atual);

    int pr_descartado;
    if (busca_em_no(&no, chave, &pr_descartado)) return ERRO; //sai se a chave ja existir

    int pos = procura_posicao(&no, chave); // descobre qual filho seguir

    int p_b_rrn, p_b_chave, p_b_pr;
    // chama o proximo nivel de baixo
    int retorno = inserir_recursivo(arv, no.P[pos], chave, pr, &p_b_rrn, &p_b_chave, &p_b_pr);

    if (retorno == ERRO || retorno == SEM_PROMOCAO) return retorno; // se nao subiu nada, so repassa

    if (no.nroChaves < 3) { // tem espaco livre no no atual
        int pos_ins = procura_posicao(&no, p_b_chave);

        for (int i = no.nroChaves; i > pos_ins; i--) { //coloca chaves pra direita
            no.C[i] = no.C[i-1];
            no.PR[i] = no.PR[i-1];
        }
        for (int i = no.nroChaves + 1; i > pos_ins + 1; i--) { //coloca ponteiros de filhos junto
            no.P[i] = no.P[i-1];
        }

        // poem os valores promovidos na vaga certa
        no.C[pos_ins] = p_b_chave;
        no.PR[pos_ins] = p_b_pr;
        no.P[pos_ins+1] = p_b_rrn;
        no.nroChaves++;

        escreve_no_arvoreB(arv, rrn_atual, &no);
        return SEM_PROMOCAO; // resolveu aqui
    } else {
        // comeco do split: o no ta lotado com 3 chaves
        int c_temp[4], pr_temp[4], p_temp[5];
        for (int i = 0; i < 3; i++) { // joga tudo que ja tinha pro vetor maior temporario
            c_temp[i] = no.C[i];
            pr_temp[i] = no.PR[i];
            p_temp[i] = no.P[i];
        }
        p_temp[3] = no.P[3];
        p_temp[4] = -1;

        int p_ins = 0;
        while (p_ins < 3 && p_b_chave > c_temp[p_ins]) p_ins++; // acha onde enfiar a chave nova no vetor temporario

        for (int i = 3; i > p_ins; i--) { // abre espaco pras chaves no temporario
            c_temp[i] = c_temp[i-1];
            pr_temp[i] = pr_temp[i-1];
        }
        for (int i = 4; i > p_ins + 1; i--) { // abre espaco pros ponteiros no temporario
            p_temp[i] = p_temp[i-1];
        }

        // insere o elemento que veio de baixo no temporario de 4 posicoes
        c_temp[p_ins] = p_b_chave;
        pr_temp[p_ins] = p_b_pr;
        p_temp[p_ins+1] = p_b_rrn;

        // escolhe o  do meio pra subir pro no pai
        *promo_chave = c_temp[2]; 
        *promo_pr = pr_temp[2];

        // arruma o no original (esquerdo), fica so com 2 chaves
        no.nroChaves = 2; 
        no.C[0] = c_temp[0]; no.PR[0] = pr_temp[0]; no.P[0] = p_temp[0];
        no.C[1] = c_temp[1]; no.PR[1] = pr_temp[1]; no.P[1] = p_temp[1];
        no.P[2] = p_temp[2];
        no.C[2] = -1; no.PR[2] = -1; no.P[3] = -1; // limpa a sobra

        int tipo_metades = (no.P[0] == -1) ? -1 : 1;
        no.tipoNo = tipo_metades; 

        // cria o no novo (direito) que recebe o resto do split
        NoArvoreB novo_no;
        inicializa_no_arvoreB(&novo_no, tipo_metades);
        novo_no.nroChaves = 1; // fica so com 1 chave (a ultima)
        novo_no.C[0] = c_temp[3]; novo_no.PR[0] = pr_temp[3];
        novo_no.P[0] = p_temp[3]; novo_no.P[1] = p_temp[4];

        CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
        int rrn_novo;
        if (cab.topo != -1) { // tenta reaproveitar no excluido da pilha
            rrn_novo = cab.topo;
            NoArvoreB removido = le_no_arvoreB(arv, rrn_novo);
            cab.topo = removido.proximo;
        } else { // senao pega o do fim do arquivo mesmo
            rrn_novo = cab.proxRRN;
            cab.proxRRN++;
        }
        cab.nroNos++;
        escreve_cabecalho_arvoreB(arv, &cab);

        *promo_filho_dir = rrn_novo; // passa o rrn do novo no pro pai ligar na direita

        // salva as duas metades separadas no arquivo
        escreve_no_arvoreB(arv, rrn_atual, &no);
        escreve_no_arvoreB(arv, rrn_novo, &novo_no);

        return PROMOCAO; // indica que teve split
    }
}

int inserir_arvoreB(FILE *arv, int chave, int pr) {
    if (arv == NULL || pr < 0) return 0;

    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    atualiza_status_arvoreB(arv, '0'); // comecou a gravar, joga pra inconsistente

    if (cab.noRaiz == -1) { // arvore vazia: cria a primeira raiz de todas
        NoArvoreB raiz;
        inicializa_no_arvoreB(&raiz, -1); // comeca como no folha (-1)
        raiz.C[0] = chave;
        raiz.PR[0] = pr;
        raiz.nroChaves = 1;

        int rrn_raiz;
        if (cab.topo != -1) { // checa pilha de excluidos
            rrn_raiz = cab.topo;
            NoArvoreB rem = le_no_arvoreB(arv, rrn_raiz);
            cab.topo = rem.proximo;
        } else {
            rrn_raiz = cab.proxRRN;
            cab.proxRRN++;
        }

        escreve_no_arvoreB(arv, rrn_raiz, &raiz);

        // atualiza as infos da raiz no cabecalho
        cab.noRaiz = rrn_raiz;
        cab.nroNos++;
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        return 1;
    }

    int promo_filho_dir, promo_chave, promo_pr;
    // dispara a busca/insercao recursiva a partir da raiz
    int retorno = inserir_recursivo(arv, cab.noRaiz, chave, pr, &promo_filho_dir, &promo_chave, &promo_pr);

    if (retorno == ERRO) { // erro ou chave duplicada, so fecha o arquivo e sai
        cab = le_cabecalho_arvoreB(arv);
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        return 1; 
    }

    if (retorno == PROMOCAO) { // o split estourou ate a raiz antiga, precisa criar uma nova raiz mais alta
        NoArvoreB nova_raiz;
        inicializa_no_arvoreB(&nova_raiz, 0); // nova raiz vai ser no interno (0)
        nova_raiz.C[0] = promo_chave;
        nova_raiz.PR[0] = promo_pr;
        nova_raiz.P[0] = cab.noRaiz; // filho esquerdo e a raiz antiga
        nova_raiz.P[1] = promo_filho_dir; // filho direito e o no criado no split
        nova_raiz.nroChaves = 1;

        cab = le_cabecalho_arvoreB(arv);
        int rrn_nova_raiz;
        if (cab.topo != -1) { // aloca RRN pra nova raiz
            rrn_nova_raiz = cab.topo;
            NoArvoreB rem = le_no_arvoreB(arv, rrn_nova_raiz);
            cab.topo = rem.proximo;
        } else {
            rrn_nova_raiz = cab.proxRRN;
            cab.proxRRN++;
        }

        escreve_no_arvoreB(arv, rrn_nova_raiz, &nova_raiz);

        // atualiza cabecalho 
        cab.noRaiz = rrn_nova_raiz;
        cab.nroNos++;
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        return 1;
    }

    cab = le_cabecalho_arvoreB(arv);
    cab.status = '1'; // encerra salvando tudo estavel
    escreve_cabecalho_arvoreB(arv, &cab);
    return 1;
}

int construir_arvoreB(FILE *arv_dados, FILE *arv_indice) {
    if (arv_dados == NULL || arv_indice == NULL) return 0;
    
    Cabecalho cab_dados = le_cabecalho(arv_dados);
    fseek(arv_dados, 17, SEEK_SET); 
    
    for (int rrn = 0; rrn < cab_dados.proxRRN; rrn++) {
        long byte_offset = 17 + ((long)rrn * 80); // tamanho fixo do registro de dados
        fseek(arv_dados, byte_offset, SEEK_SET);
        
        Registro reg;
        if (ler_registro_bin(arv_dados, &reg)) {
            if (reg.removido == '0' && reg.codEstacao != -1) { // joga no indice se for valido
                inserir_arvoreB(arv_indice, reg.codEstacao, (int)byte_offset);
            }
            libera_registro(&reg); 
        }
    }
    return 1;
}

int buscar_arvoreB(FILE *arv, int chave, int *pr) {
    if (arv == NULL || pr == NULL) return 0;
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    if (cab.noRaiz == -1) return 0;
    int rrn_atual = cab.noRaiz;

    while (rrn_atual != -1) {
        NoArvoreB no = le_no_arvoreB(arv, rrn_atual);
        if (no.nroChaves < 0 || no.nroChaves > 3) return 0; 
        if (busca_em_no(&no, chave, pr)) return 1; 
        int pos = procura_posicao(&no, chave);
        rrn_atual = no.P[pos]; // desce pro filho
    }
    return 0;
}

void empilha_no_removido(FILE *arv, CabecalhoArvoreB *cab, int rrn) {
    NoArvoreB no = le_no_arvoreB(arv, rrn);
    no.removido = '1';
    no.proximo = cab->topo;
    cab->topo = rrn;
    cab->nroNos--;
    escreve_no_arvoreB(arv, rrn, &no);
}

static void achar_sucessor(FILE *fp, int rrn, int *ch_suc, int *pr_suc) {
    NoArvoreB no = le_no_arvoreB(fp, rrn);
    while (no.P[0] != -1) {
        no = le_no_arvoreB(fp, no.P[0]);
    }
    *ch_suc = no.C[0];
    *pr_suc = no.PR[0];
}

static void remover_de_folha(NoArvoreB *no, int idx) {
    for (int i = idx; i < no->nroChaves - 1; i++) {
        no->C[i]  = no->C[i + 1];
        no->PR[i] = no->PR[i + 1];
        no->P[i + 1] = no->P[i + 2];
    }
    no->nroChaves--;
    no->C[no->nroChaves]     = -1;
    no->PR[no->nroChaves]    = -1;
    no->P[no->nroChaves + 1] = -1;
}

static void redistribuir_direita(FILE *fp, NoArvoreB *pai, int idx_esq, int rrn_esq, int rrn_dir) {
    NoArvoreB esq = le_no_arvoreB(fp, rrn_esq);
    NoArvoreB dir = le_no_arvoreB(fp, rrn_dir);

    esq.C[esq.nroChaves]    = pai->C[idx_esq];
    esq.PR[esq.nroChaves]   = pai->PR[idx_esq];
    esq.P[esq.nroChaves + 1] = dir.P[0];
    esq.nroChaves++;

    pai->C[idx_esq] = dir.C[0];
    pai->PR[idx_esq]    = dir.PR[0];

    for (int i = 0; i < dir.nroChaves - 1; i++) {
        dir.C[i]      = dir.C[i + 1];
        dir.PR[i]     = dir.PR[i + 1];
        dir.P[i]      = dir.P[i + 1];
    }
    dir.P[dir.nroChaves - 1] = dir.P[dir.nroChaves];
    dir.nroChaves--;
    dir.C[dir.nroChaves] = -1;
    dir.PR[dir.nroChaves] = -1;
    dir.P[dir.nroChaves + 1] = -1;

    escreve_no_arvoreB(fp, rrn_esq, &esq);
    escreve_no_arvoreB(fp, rrn_dir, &dir);
}

static void redistribuir_esquerda(FILE *fp, NoArvoreB *pai, int idx_dir, int rrn_esq, int rrn_dir) {
    NoArvoreB esq = le_no_arvoreB(fp, rrn_esq);
    NoArvoreB dir = le_no_arvoreB(fp, rrn_dir);

    dir.P[dir.nroChaves + 1] = dir.P[dir.nroChaves];
    for (int i = dir.nroChaves; i > 0; i--) {
        dir.C[i]      = dir.C[i - 1];
        dir.PR[i]     = dir.PR[i - 1];
        dir.P[i]      = dir.P[i - 1];
    }
    dir.C[0]  = pai->C[idx_dir - 1];
    dir.PR[0] = pai->PR[idx_dir - 1];
    dir.P[0]  = esq.P[esq.nroChaves];
    dir.nroChaves++;

    pai->C[idx_dir - 1] = esq.C[esq.nroChaves - 1];
    pai->PR[idx_dir - 1] = esq.PR[esq.nroChaves - 1];

    esq.nroChaves--;
    esq.C[esq.nroChaves]      = -1;
    esq.PR[esq.nroChaves]     = -1;
    esq.P[esq.nroChaves + 1]  = -1;

    escreve_no_arvoreB(fp, rrn_esq, &esq);
    escreve_no_arvoreB(fp, rrn_dir, &dir);
}

static void concatenar(FILE *fp, CabecalhoArvoreB *cab, NoArvoreB *pai, int idx_esq, int rrn_esq, int rrn_dir) {
    NoArvoreB esq = le_no_arvoreB(fp, rrn_esq);
    NoArvoreB dir = le_no_arvoreB(fp, rrn_dir);

    esq.C[esq.nroChaves]     = pai->C[idx_esq];
    esq.PR[esq.nroChaves]    = pai->PR[idx_esq];
    esq.P[esq.nroChaves + 1] = dir.P[0];
    esq.nroChaves++;

    for (int i = 0; i < dir.nroChaves; i++) {
        esq.C[esq.nroChaves]     = dir.C[i];
        esq.PR[esq.nroChaves]    = dir.PR[i];
        esq.P[esq.nroChaves + 1] = dir.P[i + 1];
        esq.nroChaves++;
    }

    for (int i = idx_esq; i < pai->nroChaves - 1; i++) {
        pai->C[i]      = pai->C[i + 1];
        pai->PR[i]     = pai->PR[i + 1];
        pai->P[i + 1]  = pai->P[i + 2];
    }
    pai->nroChaves--;
    pai->C[pai->nroChaves]      = -1;
    pai->PR[pai->nroChaves]     = -1;
    pai->P[pai->nroChaves + 1]  = -1;

    escreve_no_arvoreB(fp, rrn_esq, &esq);
    empilha_no_removido(fp, cab, rrn_dir);
}

static int remover_rec(FILE *fp, CabecalhoArvoreB *cab, int rrn, int chave) {
    if (rrn == -1) return 0; 

    NoArvoreB no = le_no_arvoreB(fp, rrn);

    int idx = -1;
    for (int i = 0; i < no.nroChaves; i++) {
        if (no.C[i] == chave) { idx = i; break; }
    }

    if (idx != -1) {
        if (no.P[0] == -1) {
            remover_de_folha(&no, idx);
            escreve_no_arvoreB(fp, rrn, &no);
        } else {
            int ch_suc, pr_suc;
            achar_sucessor(fp, no.P[idx + 1], &ch_suc, &pr_suc);
            no.C[idx] = ch_suc;
            no.PR[idx] = pr_suc;
            escreve_no_arvoreB(fp, rrn, &no);
            
            int underflow = remover_rec(fp, cab, no.P[idx + 1], ch_suc);
            no = le_no_arvoreB(fp, rrn); 

            if (underflow) {
                int i_filho = idx + 1;
                int rrn_filho = no.P[i_filho];

                if (i_filho + 1 <= no.nroChaves && no.P[i_filho + 1] != -1) {
                    NoArvoreB irmao = le_no_arvoreB(fp, no.P[i_filho + 1]);
                    if (irmao.nroChaves > 1) {
                        redistribuir_direita(fp, &no, i_filho, rrn_filho, no.P[i_filho + 1]);
                        escreve_no_arvoreB(fp, rrn, &no);
                        return no.nroChaves < 1 ? 1 : 0;
                    }
                }
                if (i_filho - 1 >= 0 && no.P[i_filho - 1] != -1) {
                    NoArvoreB irmao = le_no_arvoreB(fp, no.P[i_filho - 1]);
                    if (irmao.nroChaves > 1) {
                        redistribuir_esquerda(fp, &no, i_filho, no.P[i_filho - 1], rrn_filho);
                        escreve_no_arvoreB(fp, rrn, &no);
                        return no.nroChaves < 1 ? 1 : 0;
                    }
                }
                if (i_filho - 1 >= 0 && no.P[i_filho - 1] != -1) {
                    concatenar(fp, cab, &no, i_filho - 1, no.P[i_filho - 1], rrn_filho);
                    escreve_no_arvoreB(fp, rrn, &no);
                    return no.nroChaves < 1 ? 1 : 0;
                }
                if (i_filho + 1 <= no.nroChaves && no.P[i_filho + 1] != -1) {
                    concatenar(fp, cab, &no, i_filho, rrn_filho, no.P[i_filho + 1]);
                    escreve_no_arvoreB(fp, rrn, &no);
                    return no.nroChaves < 1 ? 1 : 0;
                }
            }
        }
        no = le_no_arvoreB(fp, rrn);
        return no.nroChaves < 1 ? 1 : 0;
    }

    int i = 0;
    while (i < no.nroChaves && chave > no.C[i]) i++;
    int rrn_filho = no.P[i];
    if (rrn_filho == -1) return 0; 

    int underflow = remover_rec(fp, cab, rrn_filho, chave);
    if (!underflow) return 0;

    no = le_no_arvoreB(fp, rrn);

    if (i + 1 <= no.nroChaves && no.P[i + 1] != -1) {
        NoArvoreB irmao = le_no_arvoreB(fp, no.P[i + 1]);
        if (irmao.nroChaves > 1) {
            redistribuir_direita(fp, &no, i, no.P[i], no.P[i + 1]);
            escreve_no_arvoreB(fp, rrn, &no);
            return 0;
        }
    }
    if (i - 1 >= 0 && no.P[i - 1] != -1) {
        NoArvoreB irmao = le_no_arvoreB(fp, no.P[i - 1]);
        if (irmao.nroChaves > 1) {
            redistribuir_esquerda(fp, &no, i, no.P[i - 1], no.P[i]);
            escreve_no_arvoreB(fp, rrn, &no);
            return 0;
        }
    }
    if (i - 1 >= 0 && no.P[i - 1] != -1) {
        concatenar(fp, cab, &no, i - 1, no.P[i - 1], no.P[i]);
        escreve_no_arvoreB(fp, rrn, &no);
        return no.nroChaves < 1 ? 1 : 0;
    }
    if (i + 1 <= no.nroChaves && no.P[i + 1] != -1) {
        concatenar(fp, cab, &no, i, no.P[i], no.P[i + 1]);
        escreve_no_arvoreB(fp, rrn, &no);
        return no.nroChaves < 1 ? 1 : 0;
    }

    return 0;
}

int remover_arvoreB(FILE *arv, int chave) {
    if (arv == NULL) return 0;
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    if (cab.noRaiz == -1) return 0;
    
    atualiza_status_arvoreB(arv, '0');
    
    remover_rec(arv, &cab, cab.noRaiz, chave);
    
    NoArvoreB raiz = le_no_arvoreB(arv, cab.noRaiz);
    if (raiz.nroChaves == 0 && raiz.P[0] != -1) {
        int antigo_rrn = cab.noRaiz;
        cab.noRaiz = raiz.P[0];

        NoArvoreB nova_raiz = le_no_arvoreB(arv, cab.noRaiz);
        if (nova_raiz.P[0] == -1)
            nova_raiz.tipoNo = -1;
        else
            nova_raiz.tipoNo = 0;
        escreve_no_arvoreB(arv, cab.noRaiz, &nova_raiz);

        empilha_no_removido(arv, &cab, antigo_rrn);
    } else if (raiz.nroChaves == 0 && raiz.P[0] == -1) {
        empilha_no_removido(arv, &cab, cab.noRaiz);
        cab.noRaiz = -1;
    }
    
    cab.status = '1';
    escreve_cabecalho_arvoreB(arv, &cab);
    return 1;
}