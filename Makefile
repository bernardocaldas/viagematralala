vpath %.o obj/
vpath %.h include/
vpath %.c src/

CC = gcc
CFLAGS = -Wall -ansi -pedantic -g

yasc: yasc.o stack.o
	$(CC) $(CFLAGS) -o bin/yasc obj/yasc.o obj/stack.o
	
yasc.o: yasc.c stack.h
	$(CC) $(CFLAGS) -o obj/yasc.o -I include/ -c src/yasc.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -o obj/stack.o -I include/ -c src/stack.c

clean:
	rm obj/*.o

