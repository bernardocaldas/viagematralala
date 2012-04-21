vpath %.o obj/
vpath %.h include/
vpath %.c src/

CC = gcc

CFLAGS = 

BIBS = -lpthread

all:  server cliente

server: server.o yasc.o stack.o
	$(CC) $(CFLAGS) -g -o bin/server obj/server.o obj/yasc.o obj/stack.o $(BIBS)
	
server.o: server.c yasc.h
	$(CC) $(CFLAGS) -o obj/server.o -I include/ -c src/server.c 

cliente: cliente.o
	$(CC) $(CFLAGS) -g -o bin/cliente obj/cliente.o
	
cliente.o: cliente.c
	$(CC) $(CFLAGS) -o obj/cliente.o -c src/cliente.c
	
yasc.o: yasc.c stack.h yasc.h
	$(CC) $(CFLAGS) -g -o obj/yasc.o -I include/ -c src/yasc.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -o obj/stack.o -I include/ -c src/stack.c
	


clean:
	rm obj/*.o


