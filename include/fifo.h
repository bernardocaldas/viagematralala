#ifndef _FIFO_
#define _FIFO_

#include <semaphore.h>
#include <pthread.h>


typedef struct s_node{
	int socket;
	struct s_node * next;
}fifo_node;

sem_t sem_fifo;
pthread_mutex_t mux;
fifo_node * front;
fifo_node * back;

void create_fifo(fifo_node ** front, fifo_node ** back);
fifo_node * dequeue(fifo_node ** front, fifo_node ** back);
void queue (fifo_node ** front, fifo_node ** back, int socket);

#endif

