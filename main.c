#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"
#define CODIGO_MAX 999999

static int ler_linha(const char *msg, char *buf, size_t n) {
    printf("%s", msg);
    if (fgets(buf, (int)n, stdin) == NULL) {
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}
static int ler_numero(const char *msg, double min, double max, double *saida) {
    char buf[64], *fim;
    if (!ler_linha(msg, buf, sizeof buf)) {
        return 0;
    }
    errno = 0;
    double valor = strtod(buf, &fim);
    if (fim == buf || *fim != '\0' || errno != 0 || valor < min || valor > max) {
        printf("Valor invalido (esperado entre %.0f e %.0f).\n", min, max);
        return 0;
    }
    *saida = valor;
    return 1;
}
static int ler_codigo(int *codigo) {
    double v;
    if (!ler_numero("Codigo do produto (1 a 999999): ", 1, CODIGO_MAX, &v)) {
        return 0;
    }
    if (v != (int)v) {
        printf("O codigo deve ser inteiro.\n");
        return 0;
    }
    *codigo = (int)v;
    return 1;
}
static int ler_produto(Produto *p) {
    double qtd, preco;
    if (!ler_linha("Nome: ", p->nome, sizeof p->nome) || p->nome[0] == '\0') {
        printf("Nome invalido.\n");
        return 0;
    }
    if (!ler_numero("Quantidade: ", 0, 1000000, &qtd) || qtd != (int)qtd) {
        return 0;
    }
    if (!ler_numero("Preco: ", 0, 1000000, &preco)) {
        return 0;
    }
    p->quantidade = (int)qtd;
    p->preco = (float)preco;
    return 1;
}
int main(void) {
    Tabela tabela;
    if (!tabela_criar(&tabela, CAPACIDADE_INICIAL)) {
        printf("Erro ao criar a tabela.\n");
        return 1;
    }
    char opcao[16];
    for (;;) {
        printf("\n=== Controle de estoque ===\n"
               "1) Inserir    2) Consultar   3) Atualizar\n"
               "4) Remover    5) Listar      6) Exibir estrutura\n"
               "0) Sair\n");
        if (!ler_linha("> ", opcao, sizeof opcao) || opcao[0] == '0') {
            break;
        }
        int codigo;
        Produto produto;
        switch (opcao[0]) {
        case '1':
            if (ler_codigo(&codigo) && ler_produto(&produto)) {
                int r = tabela_inserir(&tabela, codigo, produto);
                if (r == 1)      printf("Produto inserido.\n");
                else if (r == 0) printf("Ja existe produto com o codigo %d. Nada foi alterado.\n", codigo);
                else             printf("Erro ao inserir.\n");
            }
            break;
        case '2':
            if (ler_codigo(&codigo)) {
                int pos = tabela_localizar(&tabela, codigo);
                if (pos == -1) {
                    printf("Produto nao encontrado.\n");
                } else {
                    const Produto *p = &tabela.slots[pos].produto;
                    printf("%s | Qtd: %d | Preco: %.2f (indice %d)\n",
                           p->nome, p->quantidade, p->preco, pos);
                }
            }
            break;
        case '3':
            if (ler_codigo(&codigo)) {
                if (tabela_localizar(&tabela, codigo) == -1) {
                    printf("Produto nao encontrado.\n");
                } else if (ler_produto(&produto)) {
                    tabela_atualizar(&tabela, codigo, produto);
                    printf("Produto atualizado.\n");
                }
            }
            break;
        case '4':
            if (ler_codigo(&codigo)) {
                printf(tabela_remover(&tabela, codigo) ? "Produto removido.\n"
                                                       : "Produto nao encontrado.\n");
            }
            break;
        case '5':
            tabela_listar(&tabela);
            break;
        case '6':
            tabela_exibir(&tabela);
            break;
        default:
            printf("Opcao invalida.\n");
        }
    }

    tabela_destruir(&tabela);
    return 0;
}