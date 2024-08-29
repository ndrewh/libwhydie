CC=clang
all: libwhydie test
libwhydie: whydie.c Makefile
	$(CC) -o libwhydie.so --shared whydie.c libprocmaps/src/parse_procmaps.c -Ilibprocmaps/include/ -lunwind

test/test: test/test.c Makefile
	$(CC) -o test/test test/test.c
