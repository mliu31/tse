CFLAGS = -Wall -pedantic -std=c11 -I../utils -L../lib -g 
LIBS=-lutils -lcurl

lqueue_test:  lqueue_test.c
			 	gcc $(CFLAGS) $^ $(LIBS) -o $@

clean:
				rm -f lqueue_test

