# Tabela Hash — Controle de Estoque

Trabalho da Segunda Unidade — Prof. Kennedy Lopes

**Integrantes:** _[preencher: nomes do grupo]_

## 1. Configuração atribuída: 122

| Código | Escolha | Significado |
|---|---|---|
| **E1.2** | Endereçamento aberto, vetor dinâmico | Registros ficam direto no vetor, que cresce com *rehashing* |
| **H4** | Hash por dobra | A chave é dividida em partes que são somadas |
| **C2** | Sondagem linear | Em colisão, tenta a posição seguinte |
| **A2** | Controle de estoque | Chave = código do produto; valor = nome, quantidade, preço |

A tabela é implementada do zero em C: não usa nenhuma estrutura de mapa pronta, só vetores nativos, entrada/saída, temporização e alocação dinâmica.

## 2. Estrutura do repositório

```
tabela.h / tabela.c   a tabela hash (tipos, hash, inserir, buscar, remover, ...)
main.c                aplicação A2: menu de controle de estoque
testes.c              os 8 testes de correção obrigatórios
bench.c               análise experimental
Makefile              compilação e execução dos experimentos
resultados/           saída dos experimentos (gerada por `make resultados`)
slides/               slides da apresentação
```

## 3. Como compilar e executar

Requisitos: `gcc` e `make` (em Debian/Ubuntu/Mint: `sudo apt install build-essential`).

```bash
make                 # compila estoque, testes e bench
./testes             # testes de correção
./estoque            # aplicação com menu
make resultados      # roda o experimento em 4 variantes e salva em resultados/
```

`estoque` e `testes` são compilados com `-fsanitize=address` para detectar erros de memória. O benchmark é compilado com `-O2` e sem o detector, que distorceria os tempos.

## 4. Projeto

### 4.1 Estrutura de armazenamento (E1.2)

Um vetor de `Slot` alocado com `calloc`. Cada slot guarda a chave, o produto e um **estado**:

- `VAZIO`: nunca usado (valor 0, por isso `calloc` já inicializa tudo vazio);
- `OCUPADO`: contém um registro;
- `REMOVIDO`: já teve um registro (marcador de posição apagada).

A struct `Tabela` guarda o vetor, a capacidade (`total_slots`), os ocupados, os usados (ocupados + removidos), os contadores de colisões e rehashes e o número de posições visitadas pela última busca.

### 4.2 Chave e conversão

O código do produto é um **inteiro de 1 a 999999**, armazenado como `int`. A aplicação valida a entrada e recusa letras, zero, valores fora da faixa e números não inteiros. Como a chave já é numérica, não há conversão de texto para inteiro. Escolha motivada pela simplicidade da dobra e porque o enunciado não fixa o formato do código de produto.

### 4.3 Função hash por dobra (H4)

A chave é dividida em grupos de dígitos, da direita para a esquerda; os grupos são somados e se aplica o resto da divisão pela capacidade `m`:

```
h(k) = ( soma dos grupos de k ) mod m
```

O tamanho do grupo é o parâmetro `BLOCO_DOBRA` (padrão **1000**, isto é, grupos de 3 dígitos).

Exemplo com `k = 123456` e `m = 11`: grupos 456 e 123 → soma 579 → `579 mod 11 = 7`.

O resultado está sempre em `[0, m − 1]`. A análise experimental (seção 7) compara grupos de 3 e de 5 dígitos.

### 4.4 Tratamento de colisões (C2: sondagem linear)

A sequência de posições visitadas é `p_i(k) = (h(k) + i) mod m`, para `i = 0, 1, 2, ...`

- **Busca:** percorre a sequência até achar a chave ou um slot `VAZIO`. Slots `REMOVIDO` são atravessados. O laço tem no máximo `m` voltas.
- **Inserção:** primeiro verifica duplicata; depois grava no primeiro slot que não esteja `OCUPADO` (vazio ou removido).
- **Remoção:** marca o slot como `REMOVIDO`, **nunca** como `VAZIO`. Tornar vazio interromperia a busca de chaves que colidiram e foram inseridas depois.
- **Colisão (definição operacional):** conta-se uma colisão quando a posição inicialmente calculada para uma **nova** chave já está ocupada por outra. Atualizações e tentativas de inserir duplicata não contam, nem as reinserções feitas durante o rehash.

### 4.5 Crescimento e rehashing

Antes de inserir uma chave nova, se `(usados + 1) / m > LIMITE_CARGA`, a tabela **dobra** de capacidade e todos os registros `OCUPADO` são reinseridos (os índices mudam porque dependem de `m`). Os slots `REMOVIDO` são descartados no rehash. Se a alocação do vetor novo falhar, a tabela antiga permanece intacta.

O limite considera `usados` (ocupados + removidos), e não só os ocupados, porque a busca só para num slot `VAZIO`: uma tabela cheia de marcadores removidos prejudica as buscas mesmo com poucos registros ativos.

### 4.6 Políticas

| Situação | Política |
|---|---|
| Inserir chave duplicada | Recusa, devolve `0`, mantém o registro original |
| Buscar chave inexistente | Devolve `-1` (índice) / "não encontrado" na aplicação |
| Atualizar chave inexistente | Devolve `0`, nada é alterado |
| Remover chave inexistente | Devolve `0` |
| Chave inválida (≤ 0) ou erro de memória | Inserção devolve `-1` |

### 4.7 Parâmetros

| Parâmetro | Valor padrão | Onde |
|---|---|---|
| Capacidade inicial | 11 | `CAPACIDADE_INICIAL` em `tabela.h` |
| Limite de carga para crescer | 0,5 | `LIMITE_CARGA` em `tabela.h` |
| Fator de crescimento | ×2 | `tabela_inserir` |
| Bloco da dobra | 1000 (3 dígitos) | `BLOCO_DOBRA` em `tabela.h` |

## 5. Implementação (interface)

```c
int    tabela_criar(Tabela *t, int capacidade);        // 1 sucesso, 0 falha
void   tabela_destruir(Tabela *t);
int    tabela_inserir(Tabela *t, int chave, Produto p); // 1 inserido, 0 duplicata, -1 erro
int    tabela_localizar(Tabela *t, int chave);          // índice ou -1
int    tabela_atualizar(Tabela *t, int chave, Produto p); // 1 ok, 0 não existe
int    tabela_remover(Tabela *t, int chave);            // 1 ok, 0 não existe
int    tabela_rehash(Tabela *t, int nova_capacidade);
double tabela_fator_carga(const Tabela *t);
void   tabela_exibir(const Tabela *t);   // estado de cada posição (depuração)
void   tabela_listar(const Tabela *t);   // lista só os produtos
```

`tabela_exibir` mostra o cabeçalho (capacidade, ocupados, usados, colisões, rehashes, fator de carga) e, para cada posição, seu estado; chaves fora da posição de origem aparecem como `DESLOCADO`, o que evidencia as colisões.

## 6. Aplicação A2: controle de estoque

`./estoque` abre um menu com as operações obrigatórias: **inserir, consultar, atualizar, remover, listar** e **exibir estrutura**. Valores inválidos (código fora da faixa, quantidade ou preço negativos, nome vazio) são recusados com mensagem.

## 7. Testes de correção

Executados com `./testes`. Cada caso informa entrada, resultado esperado, resultado obtido e situação, e os casos 4 e 5 imprimem `tabela_exibir()` para evidenciar as colisões.

| # | Caso | Resultado esperado |
|---|---|---|
| 1 | Inserção e busca de chave existente | Inserida e encontrada com o nome correto |
| 2 | Busca de chave inexistente | Índice `-1` |
| 3 | Inserção duplicada | Retorno `0`, registro original preservado |
| 4 | Colisão demonstrada: chaves 123456, 456123 e 579 (m = 11) | Hash 7 nas três; 2 colisões; índices 7, 8, 9 |
| 5 | Remoção em colisão e busca das restantes | Remove 456123; 123456 continua no 7 e 579 no 9 |
| 6 | Atualização sem alterar a chave | Valor trocado; chave, índice e ocupados inalterados |
| 7 | Estrutura vazia e condições-limite | Operações em tabela vazia seguras; chaves 0 e −7 recusadas; `criar(0)` falha; capacidade 1 com 20 chaves funciona |
| 8 | Rehashing preserva os registros | 200 chaves a partir da capacidade 5; nenhuma perdida; α ≤ 0,5 |

Resultado: **8/8 aprovados** (conferir a linha `Resumo:` da saída de `./testes`).

As três chaves do teste 4 colidem para qualquer `m` porque a soma dos grupos de 3 dígitos é a mesma (123 + 456 = 456 + 123 = 579).

## 8. Análise experimental

### 8.1 Metodologia

- Cenários com **100, 1.000 e 10.000 registros**, **5 repetições** por cenário; os valores são médias.
- Em cada repetição, 2N códigos distintos são sorteados em [1, 999999] com semente fixa (`1000 + repetição`); os N primeiros são inseridos e os N seguintes são usados como **buscas sem sucesso**, garantidamente ausentes.
- Métricas: capacidade final, fator de carga α = n/m, colisões, tempo total (ms) das N inserções, das N buscas com sucesso e das N buscas sem sucesso, e **posições visitadas por busca** (sondagens), que não depende da máquina.
- A mesma regra de medição vale para todas as variantes. Compilação com `-O2`.
- Máquina de medição: _[preencher: CPU, memória, sistema operacional]_.

Foram comparadas quatro variantes: bloco da dobra de **3 dígitos** (`BLOCO_DOBRA = 1000`) e de **5 dígitos** (`100000`), cada uma com limite de carga **0,50** e **0,75**.

### 8.2 Resultados

**Bloco de 3 dígitos, limite 0,50**

| Registros | Cap. | α | Colisões | Ins. (ms) | Busca ok (ms) | Busca aus. (ms) | Sond. ok | Sond. aus. | Rehashes |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 100 | 352 | 0,284 | 36 | 0,0157 | 0,0012 | 0,0017 | 1,20 | 1,50 | 5 |
| 1.000 | 2.816 | 0,355 | 436 | 0,2796 | 0,0236 | 0,0620 | 3,09 | 17,26 | 8 |
| 10.000 | 22.528 | 0,444 | 9.231 | 127,3199 | 51,6748 | 118,2032 | 4.050,69 | 9.006,37 | 11 |

**Bloco de 3 dígitos, limite 0,75**

| Registros | Cap. | α | Colisões | Ins. (ms) | Busca ok (ms) | Busca aus. (ms) | Sond. ok | Sond. aus. | Rehashes |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 100 | 176 | 0,568 | 53 | 0,0108 | 0,0019 | 0,0030 | 1,67 | 2,92 | 4 |
| 1.000 | 1.408 | 0,710 | 567 | 0,1625 | 0,0248 | 0,0602 | 3,21 | 17,78 | 7 |
| 10.000 | 22.528 | 0,444 | 9.365 | 153,8847 | 54,6261 | 118,3470 | 4.050,69 | 9.006,37 | 11 |

**Bloco de 5 dígitos, limite 0,50**

| Registros | Cap. | α | Colisões | Ins. (ms) | Busca ok (ms) | Busca aus. (ms) | Sond. ok | Sond. aus. | Rehashes |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 100 | 352 | 0,284 | 33 | 0,0149 | 0,0010 | 0,0016 | 1,17 | 1,45 | 5 |
| 1.000 | 2.816 | 0,355 | 361 | 0,2684 | 0,0128 | 0,0215 | 1,28 | 1,70 | 8 |
| 10.000 | 22.528 | 0,444 | 3.622 | 2,2491 | 0,2001 | 0,2307 | 1,40 | 2,16 | 11 |

**Bloco de 5 dígitos, limite 0,75**

| Registros | Cap. | α | Colisões | Ins. (ms) | Busca ok (ms) | Busca aus. (ms) | Sond. ok | Sond. aus. | Rehashes |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 100 | 176 | 0,568 | 50 | 0,0132 | 0,0020 | 0,0036 | 1,57 | 2,80 | 4 |
| 1.000 | 1.408 | 0,710 | 560 | 0,2344 | 0,0317 | 0,0516 | 2,30 | 6,31 | 7 |
| 10.000 | 22.528 | 0,444 | 5.358 | 4,2161 | 0,2526 | 0,4157 | 1,40 | 2,16 | 11 |

### 8.3 Interpretação

**1. O tamanho do bloco da dobra domina o desempenho.** Com um código de 6 dígitos dividido em grupos de 3, a soma vale no máximo 999 + 999 = 1998, de modo que a função produz no máximo cerca de 2.000 valores de hash distintos, qualquer que seja a capacidade. Com 10.000 registros e capacidade 22.528, quase todas as chaves disputam a faixa de índices 0 a 1998, formando um agrupamento primário gigante: **4.051 sondagens por busca com sucesso e 9.006 por busca sem sucesso**, e tempo de inserção de cerca de 127 ms. Com grupos de 5 dígitos a soma chega a 100.008, as chaves se espalham, e os mesmos 10.000 registros custam **1,40 e 2,16 sondagens** e 2,2 ms de inserção (cerca de 57 vezes mais rápido; a busca sem sucesso chega a ser ~500 vezes mais rápida: 0,23 ms contra 118 ms). Com 100 registros a diferença é imperceptível, porque a faixa de 2.000 valores ainda comporta a carga.

**2. Relação com o fator de carga.** Nas variantes com bloco de 5 dígitos, as sondagens crescem com α como prevê a análise clássica da sondagem linear (≈ ½·(1 + 1/(1−α)) para busca com sucesso e ≈ ½·(1 + 1/(1−α)²) para sem sucesso). Para α = 0,444 a teoria dá ≈ 1,40 e ≈ 2,1, e foram medidos 1,40 e 2,16; para α = 0,710 a teoria dá ≈ 2,2 e ≈ 6,4, e foram medidos 2,30 e 6,31. As buscas sem sucesso crescem mais depressa porque só terminam num slot vazio.

**3. Limite de carga 0,50 × 0,75.** O limite maior economiza memória (com 1.000 registros, capacidade 1.408 contra 2.816) e faz menos rehashes (7 contra 8), mas deixa a tabela mais cheia (α = 0,71) e piora as buscas sem sucesso (6,31 contra 1,70 sondagens, com bloco de 5 dígitos). Com 10.000 registros as duas políticas terminam com a mesma capacidade (22.528), porque a capacidade só assume valores 11·2^k e o último dobro é necessário nos dois casos; por isso as sondagens coincidem e as pequenas diferenças de tempo vêm de ruído e de colisões durante o crescimento.

**4. O α final não é o limite.** Como a capacidade só dobra, o α ao final depende de onde N cai em relação à última dobra (por exemplo 0,284 com 100 registros e 0,444 com 10.000). As variantes devem ser comparadas no mesmo N.

**5. Colisões.** O número de colisões cresce com N e é maior com blocos de 3 dígitos (9.231 contra 3.622 com 10.000 registros), coerente com a concentração de hashes explicada em (1). A tabela inicial é pequena (11), então parte das colisões ocorre durante o crescimento.

**6. Tempos.** São totais de N operações, em milissegundos, medidos com `clock_gettime`. Para N = 100 os valores são muito pequenos e sujeitos a ruído; a conclusão se apoia sobretudo nas sondagens, que independem da máquina.

## 9. Decisões e limitações

- A dobra com blocos de 3 dígitos é a versão padrão do código, mas o experimento mostra que ela é inadequada para volumes maiores que cerca de 2.000 registros com este formato de código. _[Decisão do grupo: manter 3 dígitos como padrão ou adotar 5; se adotar 5, trocar `BLOCO_DOBRA` em `tabela.h` e as chaves de colisão do teste 4 para 123456, 23457 e 223455, que têm a mesma soma, 23457.]_
- O código do produto é limitado a 6 dígitos (1 a 999999). Códigos alfanuméricos exigiriam uma conversão de texto para inteiro.
- O crescimento é sempre ×2, mesmo quando a tabela está cheia de marcadores `REMOVIDO` e poucos registros ativos; uma alternativa seria refazer a tabela com a mesma capacidade para limpá-los.
- A capacidade não é restrita a números primos; a distribuição foi avaliada experimentalmente.

## 10. Conclusão

A tabela implementa a configuração 122 completa, com inserção, busca, atualização, remoção, listagem e exibição da estrutura; o *rehashing* preserva os registros; e a remoção com marcador `REMOVIDO` mantém as buscas corretas após colisões. A análise experimental mostra que o desempenho depende principalmente da distribuição da função hash (tamanho do bloco da dobra) e, em segundo plano, do limite de carga, e que as sondagens medidas acompanham o modelo teórico da sondagem linear quando a função distribui bem.
