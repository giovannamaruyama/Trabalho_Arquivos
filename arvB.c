//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#include "features.h"
#include "arvB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//cria novo arq de indice vazio
int criar_arvoreB(char *nome_arquivo) {
    //Abre arquivo modo escrita bin
    FILE *arv = fopen(nome_arquivo, "wb");
    if (arv == NULL) {
        return 0;
    }
    //inicializa cabecalho 
    CabecalhoArvoreB cab;
    inicializa_cabecalho_arvoreB(&cab);
    //escreve cab no inicio
    escreve_cabecalho_arvoreB(arv, &cab);
    fflush(arv);
    //Fecha arquivo
    fclose(arv);
    return 1;
}

//abre indice arv-b existente
FILE* abrir_arvoreB(char *nome_arquivo, char *modo) {
    //tenta abrir
    FILE *arv = fopen(nome_arquivo, modo);
    //Retorna null se falhar
    if (arv == NULL) {
        printf("Falha no processamento do arquivo\n");
        return NULL;
    }
    //verifica tamanho minimo
    fseek(arv, 0, SEEK_END);
    long tamanho = ftell(arv);
    rewind(arv);
    //arq vazio = escreve cabecalho
    if (tamanho == 0) {
        CabecalhoArvoreB cab;
        inicializa_cabecalho_arvoreB(&cab);
        escreve_cabecalho_arvoreB(arv, &cab);
        fflush(arv);
    }
    return arv;
}

//fecha arv e att status pra 1 
void fechar_arvoreB(FILE *arv, char *nome_arquivo) {
    if (arv == NULL) return;
    //status 1 consistente
    atualiza_status_arvoreB(arv, '1');
    fflush(arv);
    //fecha o arquivo
    fclose(arv);
    //Exibe conteudo
    BinarioNaTela(nome_arquivo);
}

//inicializa cab com valores padrao
void inicializa_cabecalho_arvoreB(CabecalhoArvoreB *cab) {
    cab->status = '0'; //inconsistente no inicio
    cab->noRaiz = -1;  //arvore vazia
    cab->topo = -1;  //sem nós removidos
    cab->proxRRN = 0; //prox rrn
    cab->nroNos = 0; //qtd nos 0
}

//le cabecalho
CabecalhoArvoreB le_cabecalho_arvoreB(FILE *arv) {
    CabecalhoArvoreB cab;
    //Posiciona no inicio
    fseek(arv, 0, SEEK_SET);
    //le campo a campo p/ evitar padding
    fread(&cab.status, sizeof(char), 1, arv);
    fread(&cab.noRaiz,sizeof(int),  1, arv);
    fread(&cab.topo,sizeof(int),  1, arv);
    fread(&cab.proxRRN,sizeof(int),  1, arv);
    fread(&cab.nroNos,sizeof(int),  1, arv);
    //retorna cab
    return cab;
}
 
//Escreve cabecalho
void escreve_cabecalho_arvoreB(FILE *arv, CabecalhoArvoreB *cab) {
    //posiciona no início do arquivo
    fseek(arv, 0, SEEK_SET);
    //escreve campo a campo 
    fwrite(&cab->status,sizeof(char), 1, arv);
    fwrite(&cab->noRaiz, sizeof(int),  1, arv);
    fwrite(&cab->topo,sizeof(int),  1, arv);
    fwrite(&cab->proxRRN,sizeof(int),  1, arv);
    fwrite(&cab->nroNos, sizeof(int),  1, arv);
}

//att status pra consistente
void atualiza_status_arvoreB(FILE *arv, char status) {
    //vai pro byte 0
    fseek(arv, 0, SEEK_SET);
    //Escreve status
    fwrite(&status, sizeof(char), 1, arv);
}

//inicializa no padrao
void inicializa_no_arvoreB(NoArvoreB *no, int tipo) {
    no->removido = '0'; //não removido
    no->proximo = -1;  //sem próximo na pilha
    no->tipoNo = tipo;//tipo definido
    no->nroChaves = 0; //começa sem chaves
    //Inicializa chaves com -1 
    for (int i = 0; i < MAX_CHAVES; i++) {
        no->C[i] = -1;
        no->PR[i] = -1;
    }
    //inicializa ponteiros -1 
    for (int i = 0; i < MAX_FILHOS; i++) {
        no->P[i] = -1;
    }
}

//le no pelo rrn
NoArvoreB le_no_arvoreB(FILE *arv, int rrn) {
    NoArvoreB no;
    //calc byte offset
    long byte_offset = TAM_CABECALHO_ARVORE_B + ((long)rrn * TAM_NO_ARVORE_B);
    //posiciona no
    fseek(arv, byte_offset, SEEK_SET);
    //lê campo a campo para evitar padding
    fread(&no.removido, sizeof(char), 1, arv);
    fread(&no.proximo,sizeof(int),  1, arv);
    fread(&no.tipoNo, sizeof(int),  1, arv);
    fread(&no.nroChaves,sizeof(int),  1, arv);
    //Le chaves e ref
    for (int i = 0; i < MAX_CHAVES; i++) {
        fread(&no.C[i],     sizeof(int),  1, arv);
        fread(&no.PR[i],    sizeof(int),  1, arv);
    }
    //le pt filhos
    for (int i = 0; i < MAX_FILHOS; i++) {
        fread(&no.P[i],     sizeof(int),  1, arv);
    }
    return no;
}

//escreve no na pos rrn
int escreve_no_arvoreB(FILE *arv, int rrn, NoArvoreB *no) {
    //se -1 usa prox
    if (rrn == -1) {
        fflush(arv);
        CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
        rrn = cab.proxRRN;
        cab.proxRRN++;  //incrementa
        escreve_cabecalho_arvoreB(arv, &cab);  //salva o cabeçalho
        fflush(arv);
    }
    //Calc pos
    long byte_offset = TAM_CABECALHO_ARVORE_B + ((long)rrn * TAM_NO_ARVORE_B);
    //posiciona no no
    fseek(arv, byte_offset, SEEK_SET);
    //escreve campo a campo
    fwrite(&no->removido,  sizeof(char), 1, arv);
    fwrite(&no->proximo,sizeof(int),  1, arv);
    fwrite(&no->tipoNo, sizeof(int),  1, arv);
    fwrite(&no->nroChaves,sizeof(int),  1, arv);
    //escreve chaves
    for (int i = 0; i < MAX_CHAVES; i++) {
        fwrite(&no->C[i],   sizeof(int),  1, arv);
        fwrite(&no->PR[i],  sizeof(int),  1, arv);
    }
    //Escreve filhos
    for (int i = 0; i < MAX_FILHOS; i++) {
        fwrite(&no->P[i],   sizeof(int),  1, arv);
    }
    fflush(arv);
    return rrn;
}

//procura pos p/ inserir
int procura_posicao(NoArvoreB *no, int chave) {
    //garante q no eh valido
    if (no == NULL || no->nroChaves < 0) {
        return 0;
    }
    //percorre chaves asc
    for (int i = 0; i < no->nroChaves; i++) {  
        //Chave menor q atual
        if (chave < no->C[i]) {
            //retorna i e desce
            return i;
        }
    }
     //Retorna nrochaves
    return no->nroChaves;
}

//insere no n cheio
void insere_em_no(NoArvoreB *no, int chave, int pr) {
    int pos = procura_posicao(no, chave);
    //verifica espaco
    if (no->nroChaves >= MAX_CHAVES) {
        return;  
    }
    //Desloca chaves
    for (int i = no->nroChaves; i > pos; i--) {
        no->C[i] = no->C[i-1];
        no->PR[i] = no->PR[i-1];
    }
    //desloca filhos n folha
    for (int i = no->nroChaves + 1; i > pos + 1; i--) {
        no->P[i] = no->P[i-1];
    }
    //Insere
    no->C[pos] = chave;
    no->PR[pos] = pr;
    no->nroChaves++;
}

int insere_chave_promovida(FILE *arv, int *caminho, int nivel, int chave, int pr, int rrn_filho_direito) {
    //verifica params
    if (arv == NULL || nivel < 0) {
        return 0;
    }
    //le no pai pelo historico
    int rrn_pai = caminho[nivel];
    NoArvoreB pai = le_no_arvoreB(arv, rrn_pai);

    //Pai tem espaco
    if (pai.nroChaves < MAX_CHAVES) {
        int pos = procura_posicao(&pai, chave);
        //desloca chaves e pr
        for (int i = pai.nroChaves; i > pos; i--) {
            pai.C[i] = pai.C[i-1];
            pai.PR[i] = pai.PR[i-1];
        }
        //desloca pt p/ novo filho
        for (int i = pai.nroChaves + 1; i > pos + 1; i--) {
            pai.P[i] = pai.P[i-1];
        }
        //Insere dados e filho dir
        pai.C[pos] = chave;
        pai.PR[pos] = pr;
        pai.P[pos+1] = rrn_filho_direito; 
        pai.nroChaves++;
        //escreve pai att
        escreve_no_arvoreB(arv, rrn_pai, &pai);
        return 1;
    }
    //pai cheio sobe recursao
    return split_no_arvoreB(arv, caminho, nivel, &pai, chave, pr, rrn_filho_direito);
}

int split_no_arvoreB(FILE *arv, int *caminho, int nivel, NoArvoreB *no_original, int chave_nova, int pr_novo, int rrn_filho_direito) {
    if (arv == NULL || no_original == NULL) return -1;
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    //rrn do nivel atual
    int rrn_atual = caminho[nivel];

    //Pega rrn pro novo no dir
    int rrn_novo;
    if (cab.topo != -1) {
        rrn_novo = cab.topo;
        NoArvoreB no_removido = le_no_arvoreB(arv, rrn_novo);
        cab.topo = no_removido.proximo;
    } else {
        rrn_novo = cab.proxRRN;
        cab.proxRRN++;
    }

    //vetores temp 4 chaves
    int c_temp[4], pr_temp[4], p_temp[5];
    //Copia dados originais
    for (int i = 0; i < 3; i++) {
        c_temp[i] = no_original->C[i];
        pr_temp[i] = no_original->PR[i];
        p_temp[i] = no_original->P[i];
    }
    p_temp[3] = no_original->P[3];
    p_temp[4] = -1;

    //acha pos nova chave
    int pos = 0;
    while (pos < 3 && chave_nova > c_temp[pos]) pos++;
    //desloca
    for (int i = 3; i > pos; i--) {
        c_temp[i] = c_temp[i - 1];
        pr_temp[i] = pr_temp[i - 1];
    }
    for (int i = 4; i > pos + 1; i--) {
        p_temp[i] = p_temp[i - 1];
    }
    //Insere nos arrays temp
    c_temp[pos] = chave_nova;
    pr_temp[pos] = pr_novo;
    p_temp[pos + 1] = rrn_filho_direito;

    //arruma o tipo do no
    int novo_tipo = (no_original->tipoNo == -1) ? -1 : 1;
    NoArvoreB no_esq, no_dir;
    inicializa_no_arvoreB(&no_esq, novo_tipo);
    inicializa_no_arvoreB(&no_dir, novo_tipo);

    //Distribui pra esq
    no_esq.C[0] = c_temp[0]; no_esq.PR[0] = pr_temp[0];
    no_esq.C[1] = c_temp[1]; no_esq.PR[1] = pr_temp[1];
    no_esq.P[0] = p_temp[0]; no_esq.P[1] = p_temp[1]; no_esq.P[2] = p_temp[2];
    no_esq.nroChaves = 2;

    //sobe primeira chave do segundo bloco
    int chave_promove = c_temp[2];
    int pr_promove = pr_temp[2];

    //Distribui pra dir
    no_dir.C[0] = c_temp[3]; no_dir.PR[0] = pr_temp[3];
    no_dir.P[0] = p_temp[3]; no_dir.P[1] = p_temp[4];
    no_dir.nroChaves = 1;

    //salva metades
    escreve_no_arvoreB(arv, rrn_atual, &no_esq);
    escreve_no_arvoreB(arv, rrn_novo, &no_dir);

    //Quebrou raiz original
    if (nivel == 0) {
        NoArvoreB raiz;
        inicializa_no_arvoreB(&raiz, 0); // 0 indica no Raiz
        raiz.C[0] = chave_promove;
        raiz.PR[0] = pr_promove;
        raiz.P[0] = rrn_atual;
        raiz.P[1] = rrn_novo;
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
        cab.nroNos += 2; 

        //grava cab seguro
        escreve_cabecalho_arvoreB(arv, &cab);
    } else {
        //tem pai sobe recursivo
        cab.nroNos += 1; 
        //salva inc antes recursao
        escreve_cabecalho_arvoreB(arv, &cab); 
        insere_chave_promovida(arv, caminho, nivel - 1, chave_promove, pr_promove, rrn_novo);
    }
    return rrn_novo;
}
//Funcao 1 insercao recursiva
int inserir_recursivo(FILE *arv, int rrn_atual, int chave, int pr, int *promo_filho_dir, int *promo_chave, int *promo_pr) {
    //Condicao de parada chegou no fundo falso
    //manda chave pra cima pra inserir na folha
    if (rrn_atual == -1) {
        *promo_chave = chave;
        *promo_pr = pr;
        *promo_filho_dir = -1;
        return PROMOCAO;
    }

    //Le no atual
    NoArvoreB no = le_no_arvoreB(arv, rrn_atual);

    //Procura chave e ignora duplicata
    int dummy;
    if (busca_em_no(&no, chave, &dummy)) {
        return ERRO; 
    }

    //Acha ponteiro pra descer
    int pos = procura_posicao(&no, chave);

    //Chamada recursiva descendo
    int p_b_rrn, p_b_chave, p_b_pr;
    int retorno = inserir_recursivo(arv, no.P[pos], chave, pr, &p_b_rrn, &p_b_chave, &p_b_pr);

    if (retorno == ERRO || retorno == SEM_PROMOCAO) {
        return retorno;
    }

    //Trata promocao do filho
    if (no.nroChaves < MAX_CHAVES) {
        //Tem espaco insere chave e finaliza
        int pos_ins = procura_posicao(&no, p_b_chave);

        for (int i = no.nroChaves; i > pos_ins; i--) {
            no.C[i] = no.C[i-1];
            no.PR[i] = no.PR[i-1];
        }
        for (int i = no.nroChaves + 1; i > pos_ins + 1; i--) {
            no.P[i] = no.P[i-1];
        }

        no.C[pos_ins] = p_b_chave;
        no.PR[pos_ins] = p_b_pr;
        no.P[pos_ins+1] = p_b_rrn;
        no.nroChaves++;

        escreve_no_arvoreB(arv, rrn_atual, &no);
        return SEM_PROMOCAO;
    } else {
        //No cheio faz split
        int c_temp[4], pr_temp[4], p_temp[5];
        for (int i = 0; i < 3; i++) {
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

        //Chave do meio sobe pro pai
        *promo_chave = c_temp[2];
        *promo_pr = pr_temp[2];

        //No esquerdo fica com as 2 primeiras chaves
        no.nroChaves = 2;
        no.C[0] = c_temp[0]; no.PR[0] = pr_temp[0]; no.P[0] = p_temp[0];
        no.C[1] = c_temp[1]; no.PR[1] = pr_temp[1]; no.P[1] = p_temp[1];
        no.P[2] = p_temp[2];
        
        //Limpa lixo
        no.C[2] = -1; no.PR[2] = -1; no.P[3] = -1;

        //Ajusta tipo se n era folha vira intermediario
        int novo_tipo = (no.tipoNo == -1) ? -1 : 1;
        no.tipoNo = novo_tipo; 

        //Novo no dir fica com ultima chave
        NoArvoreB novo_no;
        inicializa_no_arvoreB(&novo_no, novo_tipo);
        novo_no.nroChaves = 1;
        novo_no.C[0] = c_temp[3]; novo_no.PR[0] = pr_temp[3];
        novo_no.P[0] = p_temp[3]; novo_no.P[1] = p_temp[4];

        //Aloca rrn buscando na pilha primeiro
        CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
        int rrn_novo;
        if (cab.topo != -1) {
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

        //Grava metades no disco
        escreve_no_arvoreB(arv, rrn_atual, &no);
        escreve_no_arvoreB(arv, rrn_novo, &novo_no);

        return PROMOCAO;
    }
}

//Funcao 2 gerencia raiz e cabecalhos
int inserir_arvoreB(FILE *arv, int chave, int pr) {
    if (arv == NULL || pr < 0) return 0;

    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    atualiza_status_arvoreB(arv, '0');

    //Primeira insercao cria arvore
    if (cab.noRaiz == -1) {
        NoArvoreB raiz;
        inicializa_no_arvoreB(&raiz, -1); //raiz que eh folha eh -1
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

    //Chama recursao
    int promo_filho_dir, promo_chave, promo_pr;
    int retorno = inserir_recursivo(arv, cab.noRaiz, chave, pr, &promo_filho_dir, &promo_chave, &promo_pr);

    if (retorno == ERRO) {
        cab = le_cabecalho_arvoreB(arv);
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        return 1; //ignora duplicatas
    }

    if (retorno == PROMOCAO) {
        //Raiz quebrou cria nova pra cobrir as metades
        NoArvoreB nova_raiz;
        inicializa_no_arvoreB(&nova_raiz, 0); //tipo 0 eh da raiz
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
        return 1;
    }

    //Retorno sem promocao
    cab = le_cabecalho_arvoreB(arv);
    cab.status = '1';
    escreve_cabecalho_arvoreB(arv, &cab);
    return 1;
}



int busca_em_no(NoArvoreB *no, int chave, int *pr) {
    //valida no
    if (no == NULL || pr == NULL) {
        return 0;
    }
    //percorre chaves
    for (int i = 0; i < no->nroChaves; i++) {
        //Achou a chave
        if (no->C[i] == chave) {
            //guarda pt pro reg
            *pr = no->PR[i];
            return 1; //retorna sucesso
        }
        //chave menor q atual para
        if (chave < no->C[i]) {
            return 0; //retorna falso
        }
    }
    //Percorreu tudo e n achou
    return 0;
}

//marca no como removido logico e empilha no cabecalho
void remove_logicamente_no_arvoreB(FILE *arv, int rrn, CabecalhoArvoreB *cab) {
    if (arv == NULL || cab == NULL || rrn < 0) {
        return;
    } 
    //le no removido
    NoArvoreB no = le_no_arvoreB(arv, rrn);
    //Marca como removido
    no.removido = '1';
    //encadeia prox aponta pro antigo topo
    no.proximo = cab->topo;
    //Att topo pra esse no
    cab->topo = rrn;
    //decrementa nos ativos
    cab->nroNos--;
    //escreve no att
    escreve_no_arvoreB(arv, rrn, &no);
    //Att cabecalho
    escreve_cabecalho_arvoreB(arv, cab);
}

//retorna rrn de no removido p/ reuso
int reutiliza_no_arvoreB(FILE *arv, CabecalhoArvoreB *cab) {
    if (arv == NULL || cab == NULL) {
        return -1;
    }
    //ve se tem no removido na pilha
    if (cab->topo == -1) {
        //Pilha vazia n tem no
        return -1;
    }
    //salva rrn do topo
    int rrn_reutilizado = cab->topo;
    //le no removido p/ encadear
    NoArvoreB no_removido = le_no_arvoreB(arv, rrn_reutilizado);
    //Desempilha topo pro prox
    cab->topo = no_removido.proximo;
    //atualiza cabeçalho no arquivo
    escreve_cabecalho_arvoreB(arv, cab);
    //retorna rrn p/ reuso
    return rrn_reutilizado;
}
int construir_arvoreB(FILE *arv_dados, FILE *arv_indice) {
    if (arv_dados == NULL || arv_indice == NULL) {
        return 0;
    }
    
    // Lê o cabeçalho apenas para avançar o ponteiro (ignoramos o proxRRN dele)
    Cabecalho cab_dados;
    fseek(arv_dados, 0, SEEK_SET);
    fread(&cab_dados.status, sizeof(char), 1, arv_dados);
    fread(&cab_dados.topo, sizeof(int),  1, arv_dados);
    fread(&cab_dados.proxRRN, sizeof(int),  1, arv_dados);
    fread(&cab_dados.nroEstacoes, sizeof(int),  1, arv_dados);
    fread(&cab_dados.nroParesEstacao, sizeof(int),  1, arv_dados);
    
    Registro reg;
    int rrn = 0;
    
    // Garante que a leitura comece exatamente após o cabeçalho (Byte 17)
    fseek(arv_dados, 17, SEEK_SET);
    
    //Lê fisicamente até o final do arquivo
    while (ler_registro_bin(arv_dados, &reg)) {
        
        // Só insere se não estiver logicamente removido e tiver uma chave válida
        if (reg.removido == '0' && reg.codEstacao != -1) {
            
            //insere
            inserir_arvoreB(arv_indice, reg.codEstacao, rrn);
            
        }
        
        libera_registro(&reg);
        
        // O RRN sempre avança
        rrn++; 
    }
    
    return 1;
}

//Busca na arvore-b
int buscar_arvoreB(FILE *arv, int chave, int *pr) {
    //valida params
    if (arv == NULL || pr == NULL) {
        return 0;
    }
    //Le cabecalho pra pegar raiz
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    
    //arv vazia n tem chave
    if (cab.noRaiz == -1) {
        return 0; //nenhum no da arvore
    }
    //inicia pela raiz
    int rrn_atual = cab.noRaiz;

    //desce na arv ate achar ou chegar na folha
    while (rrn_atual != -1) {
        //Le no atual
        NoArvoreB no = le_no_arvoreB(arv, rrn_atual);

        //valida no lido
        if (no.nroChaves < 0 || no.nroChaves > MAX_CHAVES) {
            return 0; //no corrompido
        }
        //tenta achar no no atual
        if (busca_em_no(&no, chave, pr)) {
            return 1; //ACHOU
        }
        //procura por onde descer
        int pos = procura_posicao(&no, chave);

        //Pega rrn do filho
        rrn_atual = no.P[pos];
    }
    //chegou na folha e n achou
    return 0;
}
