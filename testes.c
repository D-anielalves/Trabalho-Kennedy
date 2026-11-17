#include <stdio.h>
#include <string.h>
#include "tabela.h"

static int total = 0, aprovados = 0;

static void caso(const char *nome, const char *entrada, const char *esperado,
                 const char *obtido, int ok) {
    total++;
    if (ok) aprovados++;
    printf("Teste %d: %s\n  entrada : %s\n  esperado: %s\n  obtido  : %s\n  situacao: %s\n\n",
           total, nome, entrada, esperado, obtido, ok ? "APROVADO" : "REPROVADO");
}

static Produto prod(const char *nome, int qtd, float preco) {
    Produto p = {"", qtd, preco};
    snprintf(p.nome, sizeof p.nome, "%s", nome);
    return p;
}

#define K1 123456
#define K2 456123
#define K3 579

int main(void) {
    char obt[200];
    Tabela t;
    int r;
    tabela_criar(&t, 11);
    r = tabela_inserir(&t, 1001, prod("Caneta", 10, 2.5f));
    int pos = tabela_localizar(&t, 1001);
    snprintf(obt, sizeof obt, "insercao=%d, indice=%d, nome=%s", r, pos,
             pos >= 0 ? t.slots[pos].produto.nome : "-");
    caso("Insercao e busca de chave existente", "inserir(1001, Caneta); localizar(1001)",
         "insercao=1, indice>=0, nome=Caneta", obt,
         r == 1 && pos >= 0 && strcmp(t.slots[pos].produto.nome, "Caneta") == 0);
    tabela_destruir(&t);
    tabela_criar(&t, 11);
    r = tabela_localizar(&t, 9999);
    snprintf(obt, sizeof obt, "indice=%d", r);
    caso("Busca de chave inexistente", "localizar(9999) em tabela vazia", "indice=-1", obt, r == -1);
    tabela_destruir(&t);
    tabela_criar(&t, 11);
    tabela_inserir(&t, 1001, prod("Caneta", 10, 2.5f));
    r = tabela_inserir(&t, 1001, prod("Lapis", 99, 1.0f));
    pos = tabela_localizar(&t, 1001);
    snprintf(obt, sizeof obt, "retorno=%d, ocupados=%d, nome=%s", r, t.total_ocupados, t.slots[pos].produto.nome);
    caso("Insercao duplicada", "inserir(1001, Caneta); inserir(1001, Lapis)",
         "retorno=0, ocupados=1, nome=Caneta", obt,
         r == 0 && t.total_ocupados == 1 && strcmp(t.slots[pos].produto.nome, "Caneta") == 0);
    tabela_destruir(&t);
    tabela_criar(&t, 11);
    int h1 = hash_dobra(K1, t.total_slots), h2 = hash_dobra(K2, t.total_slots), h3 = hash_dobra(K3, t.total_slots);
    tabela_inserir(&t, K1, prod("A", 1, 1));
    tabela_inserir(&t, K2, prod("B", 2, 2));
    tabela_inserir(&t, K3, prod("C", 3, 3));
    int p1 = tabela_localizar(&t, K1), p2 = tabela_localizar(&t, K2), p3 = tabela_localizar(&t, K3);
    snprintf(obt, sizeof obt, "hash=%d,%d,%d; colisoes=%d; indices=%d,%d,%d",
             h1, h2, h3, t.colisoes, p1, p2, p3);
    caso("Chaves com colisao demonstrada", "inserir 123456, 456123, 579 (m=11)",
         "hash=7,7,7; colisoes=2; indices=7,8,9", obt,
         h1 == 7 && h2 == 7 && h3 == 7 && t.colisoes == 2 && p1 == 7 && p2 == 8 && p3 == 9);
    printf("--- exibirEstrutura() ---");
    tabela_exibir(&t);
    printf("\n");
    r = tabela_remover(&t, K2);
    int a1 = tabela_localizar(&t, K1), a3 = tabela_localizar(&t, K3), a2 = tabela_localizar(&t, K2);
    snprintf(obt, sizeof obt, "remocao=%d; 123456 em %d; 579 em %d; 456123 em %d", r, a1, a3, a2);
    caso("Remocao em colisao e busca das restantes", "remover(456123), que esta no meio da cadeia",
         "remocao=1; 123456 em 7; 579 em 9; 456123 em -1", obt, r == 1 && a1 == 7 && a3 == 9 && a2 == -1);
    printf("--- exibirEstrutura() apos a remocao ---");
    tabela_exibir(&t);
    printf("\n");
    int ocup = t.total_ocupados;
    r = tabela_atualizar(&t, K3, prod("C novo", 50, 9.9f));
    int a = tabela_localizar(&t, K3);
    snprintf(obt, sizeof obt, "retorno=%d, indice=%d, qtd=%d, ocupados=%d", r, a,
             a >= 0 ? t.slots[a].produto.quantidade : -1, t.total_ocupados);
    caso("Atualizacao sem alterar a chave", "atualizar(579, qtd=50)",
         "retorno=1, indice=9, qtd=50, ocupados=2", obt,
         r == 1 && a == 9 && t.slots[a].produto.quantidade == 50 && t.total_ocupados == ocup);
    tabela_destruir(&t);
    tabela_criar(&t, 11);
    Produto x = prod("X", 1, 1);
    int v1 = tabela_localizar(&t, 5) == -1 && tabela_remover(&t, 5) == 0 && tabela_atualizar(&t, 5, x) == 0
             && t.total_ocupados == 0;
    int v2 = tabela_inserir(&t, 0, x) == -1 && tabela_inserir(&t, -7, x) == -1 && t.total_ocupados == 0;
    tabela_destruir(&t);
    int v3 = tabela_criar(&t, 0) == 0;
    tabela_criar(&t, 1);                              
    int v4 = 1;
    for (int i = 1; i <= 20; i++) v4 = v4 && tabela_inserir(&t, i, x) == 1;
    for (int i = 1; i <= 20; i++) v4 = v4 && tabela_localizar(&t, i) != -1;
    snprintf(obt, sizeof obt, "tabela vazia=%d; chave invalida=%d; capacidade 0=%d; capacidade 1 com 20 chaves=%d",
             v1, v2, v3, v4);
    caso("Estrutura vazia e condicoes-limite",
         "operacoes em tabela vazia; chaves 0 e -7; criar(0); criar(1) + 20 insercoes",
         "tabela vazia=1; chave invalida=1; capacidade 0=1; capacidade 1 com 20 chaves=1", obt,
         v1 && v2 && v3 && v4);
    tabela_destruir(&t);
    tabela_criar(&t, 5);
    int cap0 = t.total_slots, perdidos = 0;
    for (int i = 0; i < 200; i++) tabela_inserir(&t, i * 37 + 1, prod("item", i, (float)i));
    for (int i = 0; i < 200; i++) {
        int q = tabela_localizar(&t, i * 37 + 1);
        if (q == -1 || t.slots[q].produto.quantidade != i) perdidos++;
    }
    snprintf(obt, sizeof obt, "capacidade %d -> %d, rehashes=%d, ocupados=%d, perdidos=%d, alpha=%.2f",
             cap0, t.total_slots, t.rehashes, t.total_ocupados, perdidos, tabela_fator_carga(&t));
    caso("Rehashing preserva os registros", "inserir 200 chaves a partir da capacidade 5",
         "capacidade cresce, ocupados=200, perdidos=0, alpha<=0.50", obt,
         t.total_slots > cap0 && t.rehashes > 0 && t.total_ocupados == 200 && perdidos == 0
         && tabela_fator_carga(&t) <= LIMITE_CARGA);
    tabela_destruir(&t);
    printf("Resumo: %d/%d testes aprovados\n", aprovados, total);
    return aprovados == total ? 0 : 1;
}