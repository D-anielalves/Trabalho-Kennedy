#include "tabela.h"
#include <stdlib.h>
int hash_dobra(int chave, int total_slots) {
    int soma = 0;
    while (chave > 0) {
        soma += chave % 1000;
        chave /= 1000;
    }
    return soma % total_slots;
}
int tabela_criar(Tabela *tabela, int capacidade) {
    if (capacidade <= 0) {
        return 0;
    }
    tabela->slots = (Slot *)calloc(capacidade, sizeof(Slot));
    if (tabela->slots == NULL) {
        return 0;
    }
    tabela->total_slots = capacidade;
    tabela->total_ocupados = 0;
    tabela->colisoes = 0;
    tabela->rehashes = 0;

    return 1;
}
void tabela_destruir(Tabela *tabela) {
    free(tabela->slots);
    tabela->slots = NULL;
    tabela->total_slots = 0;
}