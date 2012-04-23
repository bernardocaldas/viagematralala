#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

void create_fifo(fifo_node ** front, fifo_node ** back){
	*front = NULL;
	*back = NULL;
	fifo_cnt = 0;
}

fifo_node * dequeue(fifo_node ** front) {
	fifo_node * aux;
	aux = *front;
	*front = aux->next; 
	fifo_cnt--;
	return aux;
}

fifo_node * queue (fifo_node ** front, fifo_node ** back, int socket) {
	fifo_node * new;
	new = (fifo_node*) malloc(sizeof(fifo_node));
	new->socket = socket;
	new->next = NULL;
	if(*back == NULL){
		*front = new;
		*back = new;
	}
	else
		(*back)->next = new;
	fifo_cnt ++;
}
