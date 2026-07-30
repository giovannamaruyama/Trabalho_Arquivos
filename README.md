# Sistema de Gerenciamento de Dados em Arquivos Binários

Projeto desenvolvido em linguagem C para a disciplina **SCC0215 — Organização de Arquivos**, do Instituto de Ciências Matemáticas e de Computação da Universidade de São Paulo — ICMC-USP.

O sistema realiza o armazenamento e o gerenciamento de informações sobre estações e linhas do Metrô e da CPTM em arquivos binários. Ao longo do projeto, foram implementadas operações semelhantes às encontradas em sistemas de bancos de dados, como criação, consulta, inserção, remoção, atualização, indexação, ordenação e junção de registros.

## Objetivos

O projeto teve como principais objetivos:

- compreender a organização física de dados em disco;
- implementar leitura e escrita em arquivos binários;
- gerenciar registros de tamanho fixo e variável;
- realizar remoções lógicas e reaproveitamento de espaço;
- criar um índice baseado em árvore B;
- utilizar índices para otimizar buscas;
- implementar diferentes algoritmos de junção;
- relacionar operações em arquivos com comandos da linguagem SQL.

## Tecnologias utilizadas

- Linguagem C
- Arquivos binários
- Biblioteca padrão de entrada e saída de C
- Git e GitHub

## Dados armazenados

Os registros representam conexões entre estações e linhas do sistema metroferroviário de São Paulo.

Entre os campos armazenados estão:

- código da estação;
- nome da estação;
- código da linha;
- nome da linha;
- código da próxima estação;
- distância até a próxima estação;
- código da linha de integração;
- código da estação de integração.

Os arquivos também possuem campos de controle utilizados para verificar sua consistência e gerenciar registros logicamente removidos.

## Funcionalidades

O desenvolvimento foi dividido em três etapas.

### 1. Gerenciamento de arquivos binários

A primeira etapa implementa as operações básicas de armazenamento e manipulação dos registros.

#### Criação do arquivo

Leitura dos registros de um arquivo de entrada e conversão dos dados para o formato binário definido pelo projeto.

#### Listagem dos registros

Percorre o arquivo binário e apresenta os registros armazenados, realizando o tratamento dos campos nulos.

#### Busca por campos

Permite buscar registros utilizando um ou mais critérios, incluindo campos numéricos e textuais.

#### Remoção lógica

Marca registros como removidos sem apagá-los fisicamente do arquivo e mantém uma estrutura para controlar os espaços disponíveis.

#### Inserção de registros

Insere novos registros no arquivo, considerando a estratégia de gerenciamento dos registros removidos.

#### Atualização de registros

Atualiza os campos de registros existentes. Quando o novo registro não cabe no espaço anteriormente ocupado, a versão antiga é removida logicamente e o registro atualizado é inserido em outra posição.

### 2. Indexação com árvore B

A segunda etapa acrescenta um arquivo de índice baseado em uma **árvore B de ordem 4**.

A árvore utiliza o código da estação como chave de busca e armazena referências para os registros correspondentes no arquivo de dados.

Foram implementadas as seguintes operações:

#### Criação do índice

Percorre o arquivo de dados e insere na árvore B as chaves referentes aos registros válidos.

#### Busca utilizando índice

Utiliza a árvore B para localizar registros pelo código da estação, reduzindo a quantidade de acessos ao arquivo de dados.

Para buscas por outros campos, é realizada uma busca sequencial.

#### Inserção na árvore B

Insere novas chaves no índice e trata o overflow dos nós por meio da operação de `split`.

Quando necessário, uma chave é promovida para o nó pai, mantendo as propriedades da árvore B.

#### Remoção na árvore B

Remove as chaves correspondentes aos registros removidos do arquivo de dados.

A remoção trata situações como:

- remoção em nós folha;
- substituição por chave sucessora;
- underflow;
- redistribuição de chaves;
- concatenação de nós;
- alteração da raiz;
- reaproveitamento de páginas removidas.

As páginas liberadas durante uma concatenação são armazenadas em uma pilha de páginas disponíveis e podem ser reutilizadas em inserções futuras.

### 3. Ordenação e junção de arquivos

A terceira etapa implementa operações semelhantes ao `JOIN` e ao `ORDER BY` da linguagem SQL.

A condição utilizada nas junções relaciona o código da próxima estação de um registro com o código de outra estação:

```text
estacao1.codProxEstacao = estacao2.codEstacao
