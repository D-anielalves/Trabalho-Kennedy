#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tabela.h"
#define REPS       5
#define CODIGO_MAX 999999

static double agora_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}
static void sortear_chaves(int *chaves, int n, unsigned char *usado) {
    memset(usado, 0, CODIGO_MAX + 1);
    for (int i = 0; i < n;) {
        int c = 1 + rand() % CODIGO_MAX;
        if (!usado[c]) {
            usado[c] = 1;
            chaves[i++] = c;
        }
    }
}
int main(void) {
    const int tamanhos[] = {100, 1000, 10000};
    unsigned char *usado = malloc(CODIGO_MAX + 1);
    if (usado == NULL) return 1;
    printf("Config 122: E1.2, H4 (dobra), C2 (linear), A2 (estoque)\n");
    printf("Bloco da dobra=%d | limite de carga=%.2f | capacidade inicial=%d | %d repeticoes (medias)\n\n",
           BLOCO_DOBRA, LIMITE_CARGA, CAPACIDADE_INICIAL, REPS);
    printf("%9s %8s %6s %9s %9s %9s %9s %9s %9s %7s\n",
           "Registros", "Cap.", "alpha", "Colisoes", "Ins(ms)", "BuscaOk", "BuscaAus", "SondOk", "SondAus", "Rehash");

    for (int ti = 0; ti < 3; ti++) {
        int N = tamanhos[ti];
        double cap = 0, alpha = 0, col = 0, reh = 0;
        double t_ins = 0, t_ok = 0, t_aus = 0, s_ok = 0, s_aus = 0;

        for (int rep = 0; rep < REPS; rep++) {
            srand(1000 + rep);
            int *chaves = malloc(2 * N * sizeof(int));
            sortear_chaves(chaves, 2 * N, usado);   

            Tabela t;
            tabela_criar(&t, CAPACIDADE_INICIAL);
            Produto p = {"item", 1, 1.0f};
            volatile int sink = 0;                  

            double a = agora_ms();
            for (int i = 0; i < N; i++) tabela_inserir(&t, chaves[i], p);
            double b = agora_ms();

            long sond_ok = 0, sond_aus = 0;
            for (int i = 0; i < N; i++) {
                sink += tabela_localizar(&t, chaves[i]) != -1;
                sond_ok += t.ultimas_sondagens;
            }
            double c = agora_ms();
            for (int i = N; i < 2 * N; i++) {
                sink += tabela_localizar(&t, chaves[i]) != -1;
                sond_aus += t.ultimas_sondagens;
            }
            double d = agora_ms();

            cap += t.total_slots;
            alpha += tabela_fator_carga(&t);
            col += t.colisoes;
            reh += t.rehashes;
            t_ins += b - a;  t_ok += c - b;  t_aus += d - c;
            s_ok += (double)sond_ok / N;
            s_aus += (double)sond_aus / N;

            tabela_destruir(&t);
            free(chaves);
        }
        printf("%9d %8.0f %6.3f %9.0f %9.4f %9.4f %9.4f %9.2f %9.2f %7.1f\n",
               N, cap / REPS, alpha / REPS, col / REPS, t_ins / REPS, t_ok / REPS, t_aus / REPS,
               s_ok / REPS, s_aus / REPS, reh / REPS);
    }
    printf("\nIns/BuscaOk/BuscaAus = tempo total (ms) das N operacoes.\n");
    printf("SondOk/SondAus = posicoes visitadas por busca (media).\n");
    free(usado);
    return 0;
}