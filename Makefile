vpath %.o obj/
vpath %.h include/
vpath %.c src/

CC = gcc

CFLAGS = -Wall

BIBS = -lpthread -lm

all:  yascS yascC

yascS: server.o yasc.o stack.o fifo.o pool.o manager.o protocol.o
	$(CC) $(CFLAGS) -g -o bin/yascS obj/server.o obj/yasc.o obj/stack.o obj/fifo.o obj/pool.o obj/manager.o obj/protocol.o $(BIBS)
	
server.o: server.c yasc.h pool.h
	$(CC) $(CFLAGS) -g -o obj/server.o -I include/ -c src/server.c 

yascC: client.o protocol.o fifo.o
	$(CC) $(CFLAGS) -g -o bin/yascC obj/client.o obj/protocol.o obj/fifo.o $(BIBS)
	
client.o: client.c protocol.h fifo.h
	$(CC) $(CFLAGS) -g -o obj/client.o -I include/ -c src/client.c
	
manager.o: manager.c fifo.h pool.h
	$(CC) $(CFLAGS) -g -o obj/manager.o -I include/ -c src/manager.c
	
yasc.o: yasc.c stack.h yasc.h protocol.h pool.h
	$(CC) $(CFLAGS) -g -o obj/yasc.o -I include/ -c src/yasc.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -g -o obj/stack.o -I include/ -c src/stack.c

fifo.o: fifo.c fifo.h
	$(CC) $(CFLAGS) -g -o obj/fifo.o -I include/ -c src/fifo.c

pool.o: pool.c pool.h yasc.h
	$(CC) $(CFLAGS) -g -o obj/pool.o -I include/ -c src/pool.c

protocol.o: protocol.c
	$(CC) $(CFLAGS) -g -o obj/protocol.o -I include/ -c src/protocol.c
	


clean:
	rm obj/*.o
	rm bin/*
	rm src/*.c~
	rm include/*.h~
	rm Makefile~


