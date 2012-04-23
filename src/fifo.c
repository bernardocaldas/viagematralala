#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

void create_fifo(fifo_node * front, fifo_node * back){
	front = NULL;
	back = NULL;
	counter = 0;
}

fifo_node * dequeue(fifo_node * front) {
	fifo_node * aux;
	aux = front;
	front = aux->next; 
	counter--;
	return aux;
}

fifo_node * queue (fifo_node * back, int socket) {
	fifo_node * novo;
	novo = (fifo_node*) malloc(sizeof(fifo_node);
	novo->socket = socket;
	novo->next = NULL;
	if(back == NULL)
		back = novo;
	else
		back->next = novo;
	counter ++;
}
