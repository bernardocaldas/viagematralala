#ifndef _FIFO_
#define _FIFO_

#include <semaphore.h>
#include <pthread.h>

#define FIFO_MAX 1

typedef struct s_node{
	int socket;
	int time;
	struct s_node * next;
}fifo_node;

sem_t sem_fifo_used;
sem_t sem_fifo_free;
pthread_mutex_t mux;
fifo_node * front;
fifo_node * back;

void create_fifo(fifo_node ** front, fifo_node ** back);
int dequeue(fifo_node ** front, fifo_node ** back);
void queue (fifo_node ** front, fifo_node ** back, int socket);


#endif

