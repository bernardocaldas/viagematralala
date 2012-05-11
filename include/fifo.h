#ifndef _FIFO_
#define _FIFO_

#include <semaphore.h>
#include <pthread.h>
#include <protocol.h>
#define MAX_FIFO 1

typedef struct s_item_server{
	int socket;
	int time;
}item_server;

typedef struct s_item_client{
	package tosend;
	/* the structure contains this element to prevent further conversions from ASCII to integer and viceversa */
	int ntemp;
}item_client;


typedef struct s_node{
	void * item;
	struct s_node * next;
}fifo_node;

sem_t sem_fifo_used;
sem_t sem_fifo_free;
pthread_mutex_t mux;
fifo_node * front_server;
fifo_node * back_server;
int fifo_count;


void create_fifo(fifo_node ** front, fifo_node ** back);
void * dequeue(fifo_node ** front, fifo_node ** back);
void queue (fifo_node ** front, fifo_node ** back, void * item);
int fifo_time_avg(fifo_node ** back, int current_time);


#endif

