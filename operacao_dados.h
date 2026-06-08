//Giovanna Maruyama - 16869489
//Giovanni Torres Bullo - 16869833
#ifndef OPERACAO_DADOS_H
#define OPERACAO_DADOS_H


//Funcionalidades:
void create_table(char *nome_csv, char *nome_bin); //Cria arquivo binario a partir de csv
void select_from(char *nome_bin); //Exibe todos os registros ativos
void select_from_where(char *nome_bin, int num_buscas); //Busca com n criterios
void delete_from(char *nome_bin, int num_remocoes); //Remove n registros de acordo com criterio 
void insert_into(char *nome_bin, int num_insercoes); //Insere registro reaproveitando memoria
void update_table(char *nome_bin, int num_atualizacoes); //Atualiza os registros


#endif
