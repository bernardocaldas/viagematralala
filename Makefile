vpath %.o obj/
vpath %.h include/
vpath %.c src/

CC = gcc

CFLAGS = 

BIBS = -lpthread

all: yasc server cliente

yasc: yasc.o stack.o
	$(CC) $(CFLAGS) -o bin/yasc obj/yasc.o obj/stack.o
	
yasc.o: yasc.c stack.h
	$(CC) $(CFLAGS) -o obj/yasc.o -I include/ -c src/yasc.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -o obj/stack.o -I include/ -c src/stack.c

server: server.o
	$(CC) $(CFLAGS) -o bin/server obj/server.o $(BIBS)
	
server.o: server.c
	$(CC) $(CFLAGS) -o obj/server.o -c src/server.c 

cliente: cliente.o
	$(CC) $(CFLAGS) -o bin/cliente obj/cliente.o
	
cliente.o: cliente.c
	$(CC) $(CFLAGS) -o obj/cliente.o -c src/cliente.c




clean:
	rm obj/*.o


