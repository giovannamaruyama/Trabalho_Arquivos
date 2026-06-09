//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833

#include "features.h"
#include "arvB.h"
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
    
    //Verifica se o arquivo tem tamanho minimo, pelo menos o cabecalho
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
    //atualiza status para 1 consistente
    atualiza_status_arvoreB(arv, '1');
    fflush(arv);

    //fecha o arquivo
    fclose(arv);
    //exibe conteúdo
    BinarioNaTela(nome_arquivo);
}

//Inicializa cabeçalho com valores padrão (árvore vazia)
void inicializa_cabecalho_arvoreB(CabecalhoArvoreB *cab) {
    cab->status = '0'; //arquivo inconsistente no início
    cab->noRaiz = -1;  //arvore vazia
    cab->topo = -1;  //sem nós removidos
    cab->proxRRN = 0; //próximo RRN a usar
    cab->nroNos = 0; //Quantidade de nós = 0
}
 
//Lê cabeçalho do arquivo (posição 0, 17 bytes)
CabecalhoArvoreB le_cabecalho_arvoreB(FILE *arv) {
    CabecalhoArvoreB cab;
    
    //posiciona no início do arquivo
    fseek(arv, 0, SEEK_SET);
    
    //lê campo a campo para evitar padding
    fread(&cab.status, sizeof(char), 1, arv);
    fread(&cab.noRaiz,sizeof(int),  1, arv);
    fread(&cab.topo,sizeof(int),  1, arv);
    fread(&cab.proxRRN,sizeof(int),  1, arv);
    fread(&cab.nroNos,sizeof(int),  1, arv);
    
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
    no->removido = '0'; //não está removido
    no->proximo = -1;  //sem próximo na pilha
    no->tipoNo = tipo;//tipo definido
    no->nroChaves = 0; //começa sem chaves
    
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
    //Garante que o nó eh valido
    if (no == NULL || no->nroChaves < 0) {
        return 0;
    }
    //percorre chaves em ordem crescnete
    for (int i = 0; i < no->nroChaves; i++) {   
        //se a chave eh menor que a chave atual
        if (chave < no->C[i]) {
            //retorna o indice i e desce pelo filho p[i]
            return i;
        }
    }
     //RETORNA nroChaves (DESCE PELO ÚLTIMO FILHO P[nroChaves])
    return no->nroChaves;
}

//Insere chave em nó não cheio
void insere_em_no(NoArvoreB *no, int chave, int pr) {
    int pos = procura_posicao(no, chave);
    
    // desloca chaves
    for (int i = no->nroChaves; i > pos; i--) {
        no->C[i] = no->C[i-1];
        no->PR[i] = no->PR[i-1];
    }
    
    // desloca filhos (se não é folha)
    for (int i = no->nroChaves + 1; i > pos + 1; i--) {
        no->P[i] = no->P[i-1];
    }
    
    // insere
    no->C[pos] = chave;
    no->PR[pos] = pr;
    no->nroChaves++;
}

int insere_chave_promovida(FILE *arv, int rrn_pai, int rrn_avo,int chave, int pr) {
    
    //verifica parâmetros
    if (arv == NULL || rrn_pai == -1) {
        return 0;
    }
    
    //lê o pai
    NoArvoreB pai = le_no_arvoreB(arv, rrn_pai);
    
    //caso 1: pai tem espaço
    if (pai.nroChaves < MAX_CHAVES) {
        //insere direto no pai
        insere_em_no(&pai, chave, pr);
        //escreve pai atualizado
        escreve_no_arvoreB(arv, rrn_pai, &pai);
        return 1;
    }
    
    //caso 2: pai está cheio - split recursivo
    NoArvoreB no_temp;
    memcpy(&no_temp, &pai, sizeof(NoArvoreB));
    no_temp.nroChaves = 4;
    
    //encontra posição para inserir a chave temporariamente
    int pos = procura_posicao(&no_temp, chave);
    
    //desloca chaves para direita
    for (int i = 3; i > pos; i--) {
        no_temp.C[i] = no_temp.C[i-1];
        no_temp.PR[i] = no_temp.PR[i-1];
    }
    
    //insere chave temporariamente
    no_temp.C[pos] = chave;
    no_temp.PR[pos] = pr;
    
    //lê cabeçalho para split
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    
    //cria nó direito (reutiliza ou novo)
    int rrn_novo = (cab.topo != -1) ? cab.topo : cab.proxRRN;
    if (cab.topo != -1) {
        NoArvoreB no_removido = le_no_arvoreB(arv, rrn_novo);
        cab.topo = no_removido.proximo;
    } else {
        cab.proxRRN++;
    }
    
    //cria estrutura nó direito
    NoArvoreB no_dir;
    inicializa_no_arvoreB(&no_dir, no_temp.tipoNo);
    
    //distribui 4 chaves em 2 nós
    no_dir.C[0] = no_temp.C[3];
    no_dir.PR[0] = no_temp.PR[3];
    no_dir.nroChaves = 1;
    
    no_temp.C[2] = -1;
    no_temp.PR[2] = -1;
    no_temp.nroChaves = 2;
    
    //distribui filhos (5 filhos em nó com 4 chaves)
    if (no_temp.tipoNo != -1) {  //se não é folha
        no_dir.P[0] = no_temp.P[3];
        no_dir.P[1] = no_temp.P[4];
        no_dir.P[2] = -1;
        no_dir.P[3] = -1;
        
        no_temp.P[3] = -1;
        no_temp.P[4] = -1;
    }
    
    //chave a promover
    int chave_prom = no_temp.C[2];
    int pr_prom = no_temp.PR[2];
    
    //escreve nós
    escreve_no_arvoreB(arv, rrn_pai, &no_temp);
    escreve_no_arvoreB(arv, rrn_novo, &no_dir);
    
    //se avó for raiz (não existe pai do pai)
    if (rrn_avo == -1) {
        //cria nova raiz acima
        NoArvoreB raiz;
        inicializa_no_arvoreB(&raiz, 0);  //tipoNo = 0
        
        raiz.C[0] = chave_prom;
        raiz.PR[0] = pr_prom;
        raiz.P[0] = rrn_pai; //pai esquerdo
        raiz.P[1] = rrn_novo; //pai direito
        raiz.nroChaves = 1;
        
        //escreve nova raiz
        int rrn_raiz = escreve_no_arvoreB(arv, -1, &raiz);
        
        //atualiza cabeçalho
        cab.noRaiz = rrn_raiz;
        cab.nroNos +=1;
        escreve_cabecalho_arvoreB(arv, &cab);
        
        return 1;
    }
    
    //se avó existe e não for raiz, recursão
    cab.nroNos += 1;
    escreve_cabecalho_arvoreB(arv, &cab);
    
    //insere chave promovida no avó
    return insere_chave_promovida(arv, rrn_avo, -1, chave_prom, pr_prom);
}

int split_no_arvoreB(FILE *arv, int rrn_pai, int rrn_avo, 
                     NoArvoreB *no_cheio) {
    
    //verifica parâmetros
    if (arv == NULL || no_cheio == NULL) {
        return -1;
    }
    
    //verifica se nó realmente está cheio (4 chaves)
    if (no_cheio->nroChaves != 4) {
        return -1;
    }
    
    //lê cabeçalho para ver status da pilha
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    
    //cria nó direito e reutiliza pilha se conseguir
    int rrn_novo;
    if (cab.topo != -1) {
        //reutiliza nó da pilha
        rrn_novo = cab.topo;
        NoArvoreB no_removido = le_no_arvoreB(arv, rrn_novo);
        cab.topo = no_removido.proximo;
    } else {
        //cria novo nó
        rrn_novo = cab.proxRRN;
        cab.proxRRN++;
    }
    
    //cria estrutura do nó direito
    NoArvoreB no_direito;
    inicializa_no_arvoreB(&no_direito, no_cheio->tipoNo);
    
    //nó esquerdo mantém as 2 primeiras chaves
    no_cheio->nroChaves = 2;
    
    //nó direito recebe as 2 últimas chaves
    no_direito.C[0] = no_cheio->C[3];
    no_direito.PR[0] = no_cheio->PR[3];
    no_direito.C[1] = -1;
    no_direito.PR[1] = -1;
    no_direito.nroChaves = 1;
    
    //trata os filhos (5 filhos em nó com 4 chaves)
    if (no_cheio->tipoNo != -1) {  //se não é folha
        //nó direito recebe os últimos filhos
        no_direito.P[0] = no_cheio->P[3];
        no_direito.P[1] = no_cheio->P[4];
        no_direito.P[2] = -1;
        no_direito.P[3] = -1;
        
        //nó esquerdo mantém os primeiros filhos
        no_cheio->P[3] = -1;
        no_cheio->P[4] = -1;
    }
    
    //chave promovida para o pai
    int chave_promove = no_cheio->C[2];
    int pr_promove = no_cheio->PR[2];
    
    //remove a chave promovida do nó esquerdo
    no_cheio->C[2] = -1;
    no_cheio->PR[2] = -1;
    
    //escreve nó esquerdo - captura o rrn
    int rrn_esq = escreve_no_arvoreB(arv, -1, no_cheio);
    
    //escreve nó direito
    escreve_no_arvoreB(arv, rrn_novo, &no_direito);
    
    //insere chave promovida no pai
    if (rrn_pai == -1) {
        //pai não existe - cria nova raiz
        NoArvoreB raiz;
        inicializa_no_arvoreB(&raiz, 0);  //tipoNo = 0 (raiz)
        
        raiz.C[0] = chave_promove;
        raiz.PR[0] = pr_promove;
        raiz.P[0] = rrn_esq;     //Usa RRN do nó esquerdo
        raiz.P[1] = rrn_novo;    //nó direito
        raiz.nroChaves = 1;
        
        //escreve nova raiz
        int rrn_raiz = escreve_no_arvoreB(arv, -1, &raiz);
        
        //atualiza cabeçalho com nova raiz
        cab.noRaiz = rrn_raiz;
        cab.nroNos += 2;  //adicionou 2 nós (esq e dir)
    } else {
        //pai existe - usa função auxiliar recursiva
        //não repete o split, usa a função que trata recursão
        insere_chave_promovida(arv, rrn_pai, rrn_avo, 
                              chave_promove, pr_promove);
        cab.nroNos += 2;
    }
    
    //atualiza cabeçalho
    escreve_cabecalho_arvoreB(arv, &cab);
    
    //retorna rrn da direita
    return rrn_novo;
}

int inserir_arvoreB(FILE *arv, int chave, int pr) {
    //verifica parâmetros
    if (arv == NULL || pr < 0) {
        return 0;
    }

    NoArvoreB no;
    inicializa_no_arvoreB(&no, -1);

    //lê cabeçalho
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    
    //marca arquivo como inconsistente durante operação
    atualiza_status_arvoreB(arv, '0');
    
    //caso 1: árvore vazia
    if (cab.noRaiz == -1) {
        //cria raiz (primeira chave)
        NoArvoreB raiz;
        inicializa_no_arvoreB(&raiz, 0);  //tipoNo = 0 (raiz)
        
        raiz.C[0] = chave;
        raiz.PR[0] = pr;
        raiz.nroChaves = 1;
        
        //escreve raiz no arquivo
        int rrn_raiz = escreve_no_arvoreB(arv, 0, &raiz);
        
        //atualiza cabeçalho
        cab.noRaiz = rrn_raiz;
        cab.proxRRN = 1;
        cab.nroNos = 1;
        cab.status = '1';
        
        escreve_cabecalho_arvoreB(arv, &cab);
        fflush(arv);
        
        return 1;
    }
    
    //caso 2: árvore não vazia desce até a folha 
    int rrn_atual = cab.noRaiz;
    int rrn_pai = -1;
    int rrn_avo = -1;  //rastreia avó
    
    while (rrn_atual != -1) {
        //lê nó atual
        NoArvoreB no = le_no_arvoreB(arv, rrn_atual);
        
        //verifica validade
        if (no.nroChaves < 0 || no.nroChaves > MAX_CHAVES) {
            return 0;
        }
        
        //verifica se chave já existe
        int dummy;
        if (busca_em_no(&no, chave, &dummy)) {
            //chave duplicada
            return 0;
        }
        
        //verifica se é folha
        if (no.tipoNo == -1 || (no.tipoNo == 0 && no.P[0] == -1)) {
            //é folha, sai do loop
            break;
        }
        
        //não é folha, desce para filho
        rrn_avo = rrn_pai;      //atualiza avó
        rrn_pai = rrn_atual;    //atualiza pai
        int pos = procura_posicao(&no, chave);
        rrn_atual = no.P[pos];
    }
    
    //insere na folha
    NoArvoreB folha = no;
    
    //verifica se folha tem espaço
    if (folha.nroChaves < MAX_CHAVES) {
        //folha tem espaço, insere direto
        insere_em_no(&folha, chave, pr);
        
        //escreve folha atualizada
        escreve_no_arvoreB(arv, rrn_atual, &folha);
        
        //atualiza cabeçalho
        cab.nroNos++;
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        
        fflush(arv);
        return 1;
    } else {
        //folha está cheia, faz split
        NoArvoreB no_temp;
        memcpy(&no_temp, &folha, sizeof(NoArvoreB));
        no_temp.nroChaves = 4;
        
        //insere a nova chave temporariamente
        int pos = procura_posicao(&no_temp, chave);
        for (int i = 3; i > pos; i--) {
            no_temp.C[i] = no_temp.C[i-1];
            no_temp.PR[i] = no_temp.PR[i-1];
        }
        no_temp.C[pos] = chave;
        no_temp.PR[pos] = pr;
        
        //faz split com avó 
        if (rrn_pai == -1) {
            split_no_arvoreB(arv, rrn_atual, rrn_avo, &no_temp);
        } else {
            split_no_arvoreB(arv, rrn_pai, rrn_avo, &no_temp);
        }
        //atualiza cabeçalho
        cab.status = '1';
        escreve_cabecalho_arvoreB(arv, &cab);
        
        fflush(arv);
        return 1;
    }
}
 

int busca_em_no(NoArvoreB *no, int chave, int *pr) {
    //verifica se o no eh valido
    if (no == NULL || pr == NULL) {
        return 0;
    }
    //percorre todas as chaves do no
    for (int i = 0; i < no->nroChaves; i++) {
        
        //se a chave esta presente no no
        if (no->C[i] == chave) {
            //Armazena o ponteiro para o registro
            *pr = no->PR[i];
            return 1; //retorna sucesso
        }
        
        //se a chave eh menor que a chave atual, para a busca
        if (chave < no->C[i]) {
            return 0; //retorna falso
        }
    }
    //percorreu tudo e nao encontrou:
    return 0;
}
 
//Marca nó como logicamente removido e o empilha na lista de removidos, empilha o rrn na pilha de removidos do cabeçalho
void remove_logicamente_no_arvoreB(FILE *arv, int rrn, CabecalhoArvoreB *cab) {
    //verifica parâmetros
    if (arv == NULL || cab == NULL || rrn < 0) {
        return;
    }
 
    //lê o nó a ser removido
    NoArvoreB no = le_no_arvoreB(arv, rrn);
 
    //marca o nó como removido
    no.removido = '1';
 
    //encadeia na pilha: próximo aponta para o antigo topo
    no.proximo = cab->topo;
 
    //atualiza o topo da pilha para apontar para este nó
    cab->topo = rrn;
 
    //decrementa o número de nós ativos
    cab->nroNos--;
 
    //escreve o nó atualizado no arquivo (apenas removido e proximo mudam)
    escreve_no_arvoreB(arv, rrn, &no);
 
    //atualiza o cabeçalho no arquivo
    escreve_cabecalho_arvoreB(arv, cab);
}

//retorna o RRN de um nó logicamente removido para reutilização
//identificado pelo caractere 'S' (conforme especificação)
int reutiliza_no_arvoreB(FILE *arv, CabecalhoArvoreB *cab) {
    //verifica parâmetros
    if (arv == NULL || cab == NULL) {
        return -1;
    }
 
    //verifica se existe algum nó removido na pilha
    if (cab->topo == -1) {
        //pilha vazia, nenhum nó disponível para reutilização
        return -1;
    }
 
    //salva o rrn do topo (nó a ser reutilizado)
    int rrn_reutilizado = cab->topo;
 
    //lê o nó removido para obter o encadeamento
    NoArvoreB no_removido = le_no_arvoreB(arv, rrn_reutilizado);
 
    //desempilha: atualiza topo para o próximo da pilha
    cab->topo = no_removido.proximo;
 
    //atualiza cabeçalho no arquivo
    escreve_cabecalho_arvoreB(arv, cab);
 
    //retorna o rrn disponível para reutilização
    return rrn_reutilizado;
}

//Constrói o índice Árvore-B percorrendo o arquivo de dados registro a registrs, apenas egistros não logicamente removidos têm suas chaves inseridas no índice
int construir_arvoreB(FILE *arv_dados, FILE *arv_indice) {
    if (arv_dados == NULL || arv_indice == NULL) {
        return 0;
    }
 
    //le o cabeçalho do arquivo de dados para capturar a proxRRN)
    Cabecalho cab_dados;
    fseek(arv_dados, 0, SEEK_SET);
    fread(&cab_dados.status, sizeof(char), 1, arv_dados);
    fread(&cab_dados.topo,sizeof(int),  1, arv_dados);
    fread(&cab_dados.proxRRN, sizeof(int),  1, arv_dados);
    fread(&cab_dados.nroEstacoes,sizeof(int),  1, arv_dados);
    fread(&cab_dados.nroParesEstacao, sizeof(int),  1, arv_dados);
 
    Registro reg;
 
    // Força o loop a rodar exatamente a quantidade certa de registros físicos existentes
    for (int rrn = 0; rrn < cab_dados.proxRRN; rrn++) {
        
        // Garante o alinhamento absoluto: força o ponteiro a ir para o início do registro atual antes de ler
        long byte_offset = TAM_CABECALHO + ((long)rrn * TAM_REGISTRO);
        fseek(arv_dados, byte_offset, SEEK_SET);
 
        //Faz a leitura limpa do bloco de 80 bytes
        if (ler_registro_bin(arv_dados, &reg)) {
            
            //ignora registros removidos 1 e chaves nulas -1
            if (reg.removido == '0' && reg.codEstacao != -1) {
                int resultado = inserir_arvoreB(arv_indice, reg.codEstacao, rrn);
                if (resultado == 0) {
                    libera_registro(&reg);
                    return 0; // Se houver falha crítica para
                }
            }
            libera_registro(&reg);
        }
    }
 
    return 1;
}

//busca na arvore-b
int buscar_arvoreB(FILE *arv, int chave, int *pr) {
    //verifica os parametros de entrada
    if (arv == NULL || pr == NULL) {
        return 0;
    }
    //Le o cabeçalho para obter o rrn da raiz 
    CabecalhoArvoreB cab = le_cabecalho_arvoreB(arv);
    
    //Se a arvore eh vazia a chave nao existe
    if (cab.noRaiz == -1) {
        return 0; //nenhum no da arvore
    }
    //busca iniciada pela raiz
    int rrn_atual = cab.noRaiz;
    //Loop para descer na arvore ate encontrar ou chegar na folha
    while (rrn_atual != -1) {
        //le o no atual da arvore
        NoArvoreB no = le_no_arvoreB(arv, rrn_atual);
        
        //Verifica se o no foi lido com sucesso
        if (no.nroChaves < 0 || no.nroChaves > MAX_CHAVES) {
            return 0; //no corrompido
        }
        
        //Tenta encontrar chave no no atual
        if (busca_em_no(&no, chave, pr)) {
            return 1; //se for encontrado
        }
        
        //procura por qual filho descer
        int pos = procura_posicao(&no, chave);

        //Obtem do rrn do filho por onde descer
        rrn_atual = no.P[pos];
    }
    //caso tenha percorrido ate a ultima folha e nn tiver encontrado
    return 0;
}
