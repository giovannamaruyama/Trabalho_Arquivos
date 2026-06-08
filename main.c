//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833

#include "features.h"
#include <stdio.h>

int main() {
    int func;
    char arquivo_entrada[100];
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

        case 7:

            break;
        default:
            printf("Funcionalidade invalida.\n");
            break;
        

    }

    return 0;
}