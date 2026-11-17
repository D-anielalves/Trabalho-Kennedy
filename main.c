
#include <stdio.h>
#include "tabela.h"

int main(void) {

    Tabela tabela;

    if (!tabela_criar(&tabela, 11)) {
        printf("Erro ao criar tabela!\n");
        return 1;
    }

    // Criando três produtos
    Produto p1 = {"Teclado", 10, 100.00};
    Produto p2 = {"Mouse", 15, 50.00};
    Produto p3 = {"Monitor", 5, 800.00};

    // Inserindo os produtos
    printf("Insercao:\n");

    printf("123456: %d\n", tabela_inserir(&tabela, 123456, p1));
    printf("456123: %d\n", tabela_inserir(&tabela, 456123, p2));
    printf("579: %d\n", tabela_inserir(&tabela, 579, p3));

    // Verificando colisões
    printf("\nColisoes: %d\n", tabela.colisoes);

    // Buscando os produtos
    printf("\nBusca:\n");

    printf("123456: indice %d\n",
           tabela_localizar(&tabela, 123456));

    printf("456123: indice %d\n",
           tabela_localizar(&tabela, 456123));

    printf("579: indice %d\n",
           tabela_localizar(&tabela, 579));

    // Buscando uma chave inexistente
    printf("999: indice %d\n",
           tabela_localizar(&tabela, 999));

    // Tentando inserir uma chave duplicada
    printf("\nInserindo duplicata:\n");

    printf("123456: %d\n",
           tabela_inserir(&tabela, 123456, p1));

    printf("Total ocupados: %d\n", tabela.total_ocupados);

printf("\n--- TESTE DE REMOCAO ---\n");

// 1. Remover o produto do meio
printf("Remocao: %d\n", tabela_remover(&tabela, 456123));

// 2. Conferir quantos produtos restaram
printf("Total ocupados: %d\n", tabela.total_ocupados);

// 3. Buscar os produtos
printf("Busca 123456: %d\n", tabela_localizar(&tabela, 123456));

printf("Busca 579: %d\n", tabela_localizar(&tabela, 579));

printf("Busca 456123: %d\n", tabela_localizar(&tabela, 456123));

// 4. Tentar remover novamente
printf("Remover novamente: %d\n", tabela_remover(&tabela, 456123));

// 5. Inserir um novo produto
Produto p4 = {"Fone", 20, 80.00};

printf("Inserir 1006: %d\n", tabela_inserir(&tabela, 1006, p4));

// 6. Conferir os contadores e a posição
printf("Total ocupados: %d\n", tabela.total_ocupados);
printf("Colisoes: %d\n", tabela.colisoes);
printf("Indice de 1006: %d\n", tabela_localizar(&tabela, 1006));


    tabela_destruir(&tabela);

    return 0;
}
