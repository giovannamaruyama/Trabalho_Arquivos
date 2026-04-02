//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo

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
            // Entrada esperada: 1 estacoes.csv binario.bin
            scanf("%s %s", arquivo_entrada, arquivo_saida);
            funcionalidade_1(arquivo_entrada, arquivo_saida);
            break;
            
        case 2:
            // Entrada esperada: 2 binario.bin
            scanf("%s", arquivo_entrada);
            funcionalidade_2(arquivo_entrada);
            break;
            
        default:
            printf("Funcionalidade invalida.\n");
            break;
    }

    return 0;
}