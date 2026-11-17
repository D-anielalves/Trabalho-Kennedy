#ifndef TABELA_H
#define TABELA_H

typedef enum{
    VAZIO,
    OCUPADO,
    REMOVIDO
}Estado;

typedef struct{
    char nome[100];
    int quantidade;
    float preco;
}Produto;

typedef struct{
    int chave;
    Estado estado;
    Produto produto;
}Slot;

typedef struct{
    Slot *slots;
    int total_slots;
    int total_ocupados;
    int colisoes;
    int rehashes;
}tabela;

int hash_dobra(int chave, int total_slots);

#endif