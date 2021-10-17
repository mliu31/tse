CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o

all:					$(OFILES)
							ar cr ../lib/libutils.a $(OFILES)

clean:
							rm -f *.o *.test #\'*\'
