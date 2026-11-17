CC         = gcc
CFLAGS     = -std=c11 -Wall -Wextra -g -fsanitize=address
BENCHFLAGS = -std=c11 -Wall -Wextra -O2

all: estoque testes bench

estoque: main.c tabela.c tabela.h
	$(CC) $(CFLAGS) -o $@ main.c tabela.c
testes: testes.c tabela.c tabela.h
	$(CC) $(CFLAGS) -o $@ testes.c tabela.c

# Benchmark: sem -fsanitize (ele distorce os tempos) e com -O2.
bench: bench.c tabela.c tabela.h
	$(CC) $(BENCHFLAGS) -o $@ bench.c tabela.c
bench_carga75: bench.c tabela.c tabela.h
	$(CC) $(BENCHFLAGS) -DLIMITE_CARGA=0.75 -o $@ bench.c tabela.c
bench_bloco5: bench.c tabela.c tabela.h
	$(CC) $(BENCHFLAGS) -DBLOCO_DOBRA=100000 -o $@ bench.c tabela.c
bench_bloco5_carga75: bench.c tabela.c tabela.h
	$(CC) $(BENCHFLAGS) -DBLOCO_DOBRA=100000 -DLIMITE_CARGA=0.75 -o $@ bench.c tabela.c

# Roda todas as variantes e guarda a saida em resultados/
resultados: bench bench_carga75 bench_bloco5 bench_bloco5_carga75
	mkdir -p resultados
	./bench                > resultados/bloco3_carga050.txt
	./bench_carga75        > resultados/bloco3_carga075.txt
	./bench_bloco5         > resultados/bloco5_carga050.txt
	./bench_bloco5_carga75 > resultados/bloco5_carga075.txt

clean:
	rm -f estoque testes bench bench_carga75 bench_bloco5 bench_bloco5_carga75