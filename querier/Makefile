CFLAGS = -Wall -pedantic -std=c11 -I../utils -L../lib -g 
LIBS=-lutils -lcurl

all: query query2

query:
			 gcc $(CFLAGS) query.c $(LIBS) -o $@

query2:
			 gcc $(CFLAGS) query2.c $(LIBS) -o $@

clean:
				rm -f query query2

