#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

void create_fifo(fifo_node ** front, fifo_node ** back){
	*front = NULL;
	*back = NULL;
}

int dequeue(fifo_node ** front,fifo_node ** back) {
	int socket;
	fifo_node * aux;
	aux = *front;
	*front = aux->next; 
	if(*front==NULL)
	{
		*back=NULL;
	}
	printf("Tirei da queue o aux no. %X\n", (int)aux);
	socket = aux->socket;
	free(aux);
	return socket;
}

void queue (fifo_node ** front, fifo_node ** back, int socket) {
	fifo_node * new;
	new = (fifo_node*) malloc(sizeof(fifo_node));
	new->socket = socket;
	new->time = time(NULL);
	new->next = NULL;
	if(*back == NULL){
		*front = new;
	}
	else
		(*back)->next = new;
	*back = new;
	printf("Pus na queue o new no. %X\n", (int)new);
}

void FreeFifo(fifo_node ** front){
	fifo_node * aux1, *aux2;
	aux1=*front;
	aux2=*front;
	while(aux1!=NULL)
	{
		aux2=aux1->next;
		remove_pool_node(aux1);
		aux1=aux2;
	}
}
