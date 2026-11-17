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
int tabela_localizar(Tabela *tabela, int chave) {
    int inicial = hash_dobra(chave, tabela->total_slots);
    for(int i = 0; i < tabela->total_slots; i++) {
        int posicao = (inicial + i) % tabela->total_slots;
        if (tabela->slots[posicao].estado == VAZIO) {
            return -1;
        }
        if (tabela->slots[posicao].estado == OCUPADO &&
             tabela->slots[posicao].chave == chave) {
            return posicao;
        }
    }
    return -1;
}
int tabela_inserir(Tabela *tabela, int chave, Produto produto) {
   if(chave <= 0|| tabela == NULL|| tabela->slots == NULL){
        return -1;
   }
   if (tabela_localizar(tabela, chave) != -1) {
        return 0;
    }
    int inicial = hash_dobra(chave, tabela->total_slots);
    if (tabela->slots[inicial].estado == OCUPADO){
        tabela->colisoes++;
    }
    for(int i = 0; i < tabela->total_slots; i++) {
        int posicao = (inicial + i) % tabela->total_slots;
        if (tabela->slots[posicao].estado != OCUPADO) {
            tabela->slots[posicao].chave = chave;
            tabela->slots[posicao].produto = produto;
            tabela->slots[posicao].estado = OCUPADO;
            tabela->total_ocupados++;
            return 1;
        }
    }
    return -1;
}