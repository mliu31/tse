CFLAGS = -Wall -pedantic -std=c11 -I../utils -L../lib -g 
LIBS=-lutils -lcurl


all: lqueue_test lqueue_test2 lqueue_test3

lqueue_test:  lqueue_test.c
			 	gcc $(CFLAGS) $^ $(LIBS) -o $@ -lpthread 

lqueue_test2:  lqueue_test2.c
			 	gcc $(CFLAGS) $^ $(LIBS) -o $@ -lpthread

lqueue_test3:  lqueue_test3.c
			 	gcc $(CFLAGS) $^ $(LIBS) -o $@ -lpthread

clean:
				rm -f lqueue_test lqueue_test2 lqueue_test3

