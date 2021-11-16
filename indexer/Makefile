CFLAGS = -Wall -pedantic -std=c11 -I../utils -L../lib -g 
LIBS=-lutils -lcurl

all: indexer lindexer 0threads

indexer:
			 gcc $(CFLAGS) indexer.c $(LIBS) -o $@

lindexer:
			 gcc $(CFLAGS) lindexer.c $(LIBS) -o $@ -lpthread

0threads:
			 gcc $(CFLAGS) lindexer-0threads.c $(LIBS) -o $@ -lpthread	

clean:
				rm -f indexer lindexer 0threads

