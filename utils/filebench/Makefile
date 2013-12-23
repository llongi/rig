CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -Wno-pointer-sign -march=native -O2 -pipe
LIBS=-lrig -lrt -D_XOPEN_SOURCE=600 -I../../trunk/src/str_ops/

all: file random1k

file:
	$(CC) $(CFLAGS) $(LIBS) -o file_bench file_search_benchmark.c

random1k:
	$(CC) $(CFLAGS) $(LIBS) -o random1k_bench random1k_search_benchmark.c

clean:
	rm -f file_bench random1k_bench
