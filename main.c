#include "tabela.h"
#include <stdio.h>

int main() {
    Tabela tabela;
    if (!tabela_criar(&tabela, 11)) {
        printf("Falha ao criar a tabela.\n");
        return 1;
    }
    printf("Tabela criada com sucesso. Capacidade: %d\n", tabela.total_slots);
    printf("Estado do slot 0: %d\n", tabela.slots[0].estado);
    printf("Estado do slot 10: %d\n", tabela.slots[10].estado);
    return 0;
}