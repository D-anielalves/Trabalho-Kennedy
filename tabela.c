#include "tabela.h"

int hash_dobra(int chave, int total_slots) {
    int soma = 0;
    while (chave > 0) {
        soma += chave % 1000;
        chave /= 1000;
    }
    return soma % total_slots;
}