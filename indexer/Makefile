CFLAGS = -Wall -pedantic -std=c11 -I../utils -L../lib -g 
LIBS=-lutils -lcurl

all: indexer 0threads 1thread

indexer:
			 gcc $(CFLAGS) indexer.c $(LIBS) -o $@


0threads:
			 gcc $(CFLAGS) lindexer-0threads.c $(LIBS) -o $@ -lpthread	

1thread:
			 gcc $(CFLAGS) lindexer-1thread.c $(LIBS) -o $@ -lpthread	

clean:
				rm -f indexer 0threads 1thread
