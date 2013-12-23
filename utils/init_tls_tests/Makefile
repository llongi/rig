THREADS=4
CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -march=native -O2 -pipe

all: constdest tls tid

constdest:
	$(CC) $(CFLAGS) -fPIC -c -o constdestlib.o const_dest_lib.c
	$(CC) $(CFLAGS) -shared -o libconstdest.so constdestlib.o
	$(CC) $(CFLAGS) -L. -lconstdest -Wl,-rpath,. -o cd_loadtime lib_user_loadtime.c -DCALL_FOO=1
	$(CC) $(CFLAGS) -L. -lconstdest -Wl,-rpath,. -o cd_loadtime_nocall lib_user_loadtime.c -DCALL_FOO=0
	$(CC) $(CFLAGS) -ldl -o cd_runtime lib_user_runtime.c -DCALL_FOO=1
	$(CC) $(CFLAGS) -ldl -o cd_runtime_nocall lib_user_runtime.c -DCALL_FOO=0

tls:
	$(CC) $(CFLAGS) -lpthread -o tls_test thread_tls.c -DNUM_THREADS=$(THREADS)

tid:
	$(CC) $(CFLAGS) -lrig -o tid_test thread_tid.c -DNUM_THREADS=$(THREADS)

clean:
	rm -f constdestlib.o libconstdest.so
	rm -f cd_loadtime cd_loadtime_nocall
	rm -f cd_runtime cd_runtime_nocall
	rm -f tls_test tid_test
