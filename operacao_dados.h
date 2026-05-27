#ifndef OPERACAO_DADOS_H
#define OPERACAO_DADOS_H

//Funcionalidades:
void funcionalidade_1(char *nome_csv, char *nome_bin); //Cria arquivo binario a partir de csv
void funcionalidade_2(char *nome_bin); //Exibe todos os registros ativos
void funcionalidade_3(char *nome_bin, int num_buscas); //Busca com n criterios
void funcionalidade_4(char *nome_bin, int num_remocoes); //Remove n registros de acordo com criterio 
void funcionalidade_5(char *nome_bin, int num_insercoes); //Insere registro reaproveitando memoria
void funcionalidade_6(char *nome_bin, int num_atualizacoes); //Atualiza os registros


#endif
