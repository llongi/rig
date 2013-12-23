CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -Wno-pointer-sign -march=native -O2 -pipe
LIBS=-lm -lrt -D_XOPEN_SOURCE=600

all: strstr rstrstr strchr rstrchr strlen

strstr:
	$(CC) $(CFLAGS) $(LIBS) -o strstr strstrbench.c

rstrstr:
	$(CC) $(CFLAGS) $(LIBS) -o rstrstr rstrstrbench.c

strchr:
	$(CC) $(CFLAGS) $(LIBS) -o strchr strchrbench.c

rstrchr:
	$(CC) $(CFLAGS) $(LIBS) -o rstrchr rstrchrbench.c

strlen:
	$(CC) $(CFLAGS) $(LIBS) -o strlen strlenbench.c

clean:
	rm -f strstr rstrstr strchr rstrchr strlen
