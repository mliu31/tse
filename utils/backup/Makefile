CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o

all:					$(OFILES)
							ar cr ../lib/libutils.a $(OFILES)

%.o:					%.c %.h
							gcc $(CFLAGS) -c $<

test1.test:   queue.o test1.o
							gcc $(CFLAGS) queue.o test1.o -o $@

clean:
							rm -f *.o *.test #\'*\'
