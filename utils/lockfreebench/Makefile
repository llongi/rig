ifndef THREADS
	THREADS=8
endif
ifndef BENCH_RUNS
	BENCH_RUNS=10
endif

ifdef TCMALLOC
	MALLOC=-ltcmalloc
endif
ifdef JEMALLOC
	MALLOC=-ljemalloc
endif

CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -march=native -O2 -pipe -ggdb
LIBS=-lrig -lrt -lm -D_XOPEN_SOURCE=600 $(MALLOC) \
	-DNUM_THREADS=$(THREADS) -DNUM_BENCH_RUNS=$(BENCH_RUNS)

all: list queue stack

list:
	$(CC) $(CFLAGS) $(LIBS) -o list_bench list_benchmark.c
	$(CC) $(CFLAGS) $(LIBS) -o list_bench_randp list_benchmark_randp.c

queue:
	$(CC) $(CFLAGS) $(LIBS) -o queue_bench queue_benchmark.c
	$(CC) $(CFLAGS) $(LIBS) -o queue_bench_randp queue_benchmark_randp.c

stack:
	$(CC) $(CFLAGS) $(LIBS) -o stack_bench stack_benchmark.c
	$(CC) $(CFLAGS) $(LIBS) -o stack_bench_randp stack_benchmark_randp.c

clean:
	rm -f list_bench list_bench_randp
	rm -f queue_bench queue_bench_randp
	rm -f stack_bench stack_bench_randp
