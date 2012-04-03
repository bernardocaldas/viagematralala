vpath %.o obj/
vpath %.h include/
vpath %.c src/

CC = gcc
CFLAGS = -Wall -ansi -pedantic

yasc: yasc.o 
	$(CC) $(CFLAGS) -o bin/yasc obj/yasc.o
	
yasc.o: yasc.c
	$(CC) $(CFLAGS) -o obj/yasc.o -I include/ -c src/yasc.c

clean:
	rm obj/*.o

