#include "tabela.h"
#include <stdio.h>
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
    if (tabela == NULL || capacidade <= 0) {
        return 0;
    }
    tabela->slots = (Slot *)calloc(capacidade, sizeof(Slot));
    if (tabela->slots == NULL) {
        return 0;
    }
    tabela->total_slots = capacidade;
    tabela->total_ocupados = 0;
    tabela->total_usados = 0;
    tabela->colisoes = 0;
    tabela->rehashes = 0;
    return 1;
}
void tabela_destruir(Tabela *tabela) {
    if (tabela == NULL) {
        return;
    }
    free(tabela->slots);
    tabela->slots = NULL;
    tabela->total_slots = 0;
    tabela->total_ocupados = 0;
    tabela->total_usados = 0;
}
int tabela_localizar(Tabela *tabela, int chave) {
    if (tabela == NULL || tabela->slots == NULL || tabela->total_slots == 0) {
        return -1;
    }
    int inicial = hash_dobra(chave, tabela->total_slots);
    for (int i = 0; i < tabela->total_slots; i++) {
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
static int tabela_colocar(Tabela *tabela, int chave, Produto produto) {
    int inicial = hash_dobra(chave, tabela->total_slots);
    for (int i = 0; i < tabela->total_slots; i++) {
        int posicao = (inicial + i) % tabela->total_slots;
        if (tabela->slots[posicao].estado != OCUPADO) {
            if (tabela->slots[posicao].estado == VAZIO) {
                tabela->total_usados++;  
            }
            tabela->slots[posicao].chave = chave;
            tabela->slots[posicao].produto = produto;
            tabela->slots[posicao].estado = OCUPADO;
            tabela->total_ocupados++;
            return 1;
        }
    }
    return 0;
}
int tabela_rehash(Tabela *tabela, int nova_capacidade) {
    if (tabela == NULL || nova_capacidade <= tabela->total_ocupados) {
        return 0;
    }
    Slot *novos = (Slot *)calloc(nova_capacidade, sizeof(Slot));
    if (novos == NULL) {
        return 0;
    }

    Slot *antigos = tabela->slots;
    int capacidade_antiga = tabela->total_slots;

    tabela->slots = novos;
    tabela->total_slots = nova_capacidade;
    tabela->total_ocupados = 0;
    tabela->total_usados = 0;

    for (int i = 0; i < capacidade_antiga; i++) {
        if (antigos[i].estado == OCUPADO) {
            tabela_colocar(tabela, antigos[i].chave, antigos[i].produto);
        }
    }
    free(antigos); 
    tabela->rehashes++;
    return 1;
}
int tabela_inserir(Tabela *tabela, int chave, Produto produto) {
    if (chave <= 0 || tabela == NULL || tabela->slots == NULL) {
        return -1;
    }
    if (tabela_localizar(tabela, chave) != -1) {
        return 0;
    }

    if ((double)(tabela->total_usados + 1) / tabela->total_slots > LIMITE_CARGA) {
        if (!tabela_rehash(tabela, tabela->total_slots * 2)) {
            return -1;
        }
    }

    int inicial = hash_dobra(chave, tabela->total_slots);
    if (tabela->slots[inicial].estado == OCUPADO) {
        tabela->colisoes++;
    }
    return tabela_colocar(tabela, chave, produto) ? 1 : -1;
}
int tabela_remover(Tabela *tabela, int chave) {
    int posicao = tabela_localizar(tabela, chave);
    if (posicao == -1) {
        return 0;
    }
    tabela->slots[posicao].estado = REMOVIDO;
    tabela->total_ocupados--;
    return 1;
}
int tabela_atualizar(Tabela *tabela, int chave, Produto produto) {
    int posicao = tabela_localizar(tabela, chave);
    if (posicao == -1) {
        return 0;
    }
    tabela->slots[posicao].produto = produto;
    return 1;
}
double tabela_fator_carga(const Tabela *tabela) {
    if (tabela == NULL || tabela->total_slots == 0) {
        return 0.0;
    }
    return (double)tabela->total_ocupados / tabela->total_slots;
}

void tabela_exibir(const Tabela *tabela) {
    if (tabela == NULL || tabela->slots == NULL) {
        return;
    }
    printf("\n=========== Tabela Hash ===========\n");
    printf("capacidade: %d\n", tabela->total_slots);
    printf("ocupados: %d\n", tabela->total_ocupados);
    printf("usados (ocupados + removidos): %d\n", tabela->total_usados);
    printf("colisoes: %d\n", tabela->colisoes);
    printf("rehashes: %d\n", tabela->rehashes);
    printf("fator de carga: %.2f\n", tabela_fator_carga(tabela));
    printf("====================================\n");

    for (int i = 0; i < tabela->total_slots; i++) {
        const Slot *slot = &tabela->slots[i];
        printf("indice %d: ", i);
        if (slot->estado == VAZIO) {
            printf("VAZIO\n");
        } else if (slot->estado == REMOVIDO) {
            printf("REMOVIDO\n");
        } else {
            int origem = hash_dobra(slot->chave, tabela->total_slots);
            printf("OCUPADO | Origem: %d | Chave: %d | Produto: %s | Qtd: %d | Preco: %.2f",
                   origem, slot->chave, slot->produto.nome,
                   slot->produto.quantidade, slot->produto.preco);
            if (origem != i) {
                printf(" | DESLOCADO");
            }
            printf("\n");
        }
    }
}
void tabela_listar(const Tabela *tabela) {
    if (tabela == NULL || tabela->slots == NULL || tabela->total_ocupados == 0) {
        printf("(estoque vazio)\n");
        return;
    }
    printf("%-8s %-24s %8s %10s\n", "Codigo", "Nome", "Qtd", "Preco");
    for (int i = 0; i < tabela->total_slots; i++) {
        const Slot *slot = &tabela->slots[i];
        if (slot->estado == OCUPADO) {
            printf("%-8d %-24s %8d %10.2f\n", slot->chave, slot->produto.nome,
                   slot->produto.quantidade, slot->produto.preco);
        }
    }
}