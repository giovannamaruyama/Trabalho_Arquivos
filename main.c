//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833

#include "feature.h"
#include "cabecalho.h"
#include "registro.h"
#include "auxcsv.h"
#include "indice.h"
#include "juncao.h"
#include <stdio.h>

int main() {
    int func;
    char arquivo_entrada[100];
    char arquivo_arvoreB[100];
    char arquivo_saida[100];

    if (scanf("%d", &func) != 1) { //le o indicador de funcionalidade
        return 0;
    }

    switch(func) {
        case 1: //funcionalidade 1 cria arq binário a partir de csv
            scanf("%s %s", arquivo_entrada, arquivo_saida);
            create_table(arquivo_entrada, arquivo_saida);
            break;
            
        case 2: //funcionalidade 2 exibe todos os registros ativos
            scanf("%s", arquivo_entrada);
            select_from(arquivo_entrada);
            break;
        
        case 3: //funcionalidade 3 busca com m criterios
            int m;
            scanf("%s %d", arquivo_entrada, &m);
            select_from_where(arquivo_entrada, m);
            break;
            
        case 4: //funcionalidade 4 remocao logica
            int n;
            scanf("%s %d", arquivo_entrada, &n);
            delete_from(arquivo_entrada, n);
            break;

         case 5: //funcionalidade 5 insercao com reaproveitamento
            int p;
            scanf("%s %d", arquivo_entrada, &p);
            insert_into(arquivo_entrada, p);
            break;

        case 6: 
            int q; //funcionalidade 6 atualizacao
            scanf("%s %d", arquivo_entrada, &q);
            update_table(arquivo_entrada, q);
            break;

        case 7: //funcionalidade 7- constroi indice
            scanf("%s %s", arquivo_entrada, arquivo_arvoreB);
            create_idx(arquivo_entrada, arquivo_arvoreB);
            break;
            break;
        case 8: //funcionalidade 8 -busca com índice
            scanf("%s %s", arquivo_entrada, arquivo_arvoreB);
            select_from_idx(arquivo_entrada, arquivo_arvoreB);
            break;
        case 9: // funcionalidade 9 - inserção em dados e índice
            int num_insercoes;
            scanf("%s %s %d", arquivo_entrada, arquivo_arvoreB, &num_insercoes);
            insert_idx(arquivo_entrada, arquivo_arvoreB, num_insercoes);
            break;
        case 10: // funcionalidade 10 - remoção com índice
            int num_remocoes;
            scanf("%s %s %d", arquivo_entrada, arquivo_arvoreB, &num_remocoes);
            delete_from_idx(arquivo_entrada, arquivo_arvoreB, num_remocoes);
            break; 
        case 11: { //funcionalidade 11 merge por loop aninhado
            char campo1[50], campo2[50], arq2[100];
            scanf("%s %s %s %s", arquivo_entrada, campo1, arq2, campo2);
            nested_loop_join(arquivo_entrada, arq2);
            break;
        }
        case 12: { //funcionalidade 12: merge por loop único com índice árvore-B
            char campo1[50], campo2[50], arq2[100], arq_idx[100];
            scanf("%s %s %s %s %s", arquivo_entrada, campo1, arq2, campo2, arq_idx);
            loop_unico_join(arquivo_entrada, arq2, arq_idx);
            break;
}
        default:
            printf("Funcionalidade invalida.\n");
            break;
    }

    return 0;
}