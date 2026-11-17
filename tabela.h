#ifndef TABELA_H
#define TABELA_H
#define CAPACIDADE_INICIAL 11
#define LIMITE_CARGA 0.5 

typedef enum {
    VAZIO,
    OCUPADO,
    REMOVIDO
} Estado;

typedef struct {
    char nome[100];
    int quantidade;
    float preco;
} Produto;

typedef struct {
    int chave;
    Estado estado;
    Produto produto;
} Slot;

typedef struct {
    Slot *slots;
    int total_slots;     
    int total_ocupados;  
    int total_usados;   
    int colisoes;
    int rehashes;
} Tabela;

int hash_dobra(int chave, int total_slots);
int tabela_criar(Tabela *tabela, int capacidade);
void tabela_destruir(Tabela *tabela);
int tabela_localizar(Tabela *tabela, int chave);
int tabela_inserir(Tabela *tabela, int chave, Produto produto);
int tabela_remover(Tabela *tabela, int chave);
int tabela_atualizar(Tabela *tabela, int chave, Produto produto);
int tabela_rehash(Tabela *tabela, int nova_capacidade);
double tabela_fator_carga(const Tabela *tabela);
void tabela_exibir(const Tabela *tabela);
void tabela_listar(const Tabela *tabela);

#endif