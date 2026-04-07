//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833

#include "features.h"
#include <stdio.h>

int main() {
    int func;
    char arquivo_entrada[100];
    char arquivo_saida[100];

    if (scanf("%d", &func) != 1) {
        return 0;
    }

    switch(func) {
        case 1:
            scanf("%s %s", arquivo_entrada, arquivo_saida);
            funcionalidade_1(arquivo_entrada, arquivo_saida);
            break;
            
        case 2:
            scanf("%s", arquivo_entrada);
            funcionalidade_2(arquivo_entrada);
            break;
        
        case 3: 
        int m;
        scanf("%s %d", arquivo_entrada, &m);
        funcionalidade_3(arquivo_entrada, m);
        break;
        
        case 4: 
            int n;
            scanf("%s %d", arquivo_entrada, &n);
            funcionalidade_4(arquivo_entrada, n);
            break;

         case 5: 
            int p;
            scanf("%s %d", arquivo_entrada, &p);
            funcionalidade_5(arquivo_entrada, p);
            break;

        case 6: 
            int q;
            scanf("%s %d", arquivo_entrada, &q);
            funcionalidade_6(arquivo_entrada, q);
            break;

        default:
            printf("Funcionalidade invalida.\n");
            break;

    }

    return 0;
}