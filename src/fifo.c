#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

void create_fifo(fifo_node ** front, fifo_node ** back){
	*front = NULL;
	*back = NULL;
}

void * dequeue(fifo_node ** front,fifo_node ** back) {
	item_server * item;
	fifo_node * aux;
	aux = *front;
	*front = aux->next; 
	if(*front==NULL)
	{
		*back=NULL;
	}
	printf("Tirei da queue o aux no. %X\n", (int)aux);
	item = (item_server*) aux->item;
	free(aux);
	return item;
}

void queue (fifo_node ** front, fifo_node ** back, void * item) {
	fifo_node * new;
	
	new = (fifo_node*) malloc(sizeof(fifo_node));
	/* takes for granted previous memory allocation */
	new->item = item;
	new->next = NULL;
	if(*back == NULL){
		*front = new;
	}
	else
		(*back)->next = new;
	*back = new;
	printf("Pus na queue o new no. %X\n", (int)new);
}

void remove_fifo_node(fifo_node * aux)
{
	/*Como é que eu sei que tipo de fifo_node é que é?*/
	free(aux->item);
	free(aux);
}

void FreeFifo(fifo_node ** front){
	fifo_node * aux1, *aux2;
	aux1=*front;
	aux2=*front;
	while(aux1!=NULL)
	{
		aux2=aux1->next;
		remove_fifo_node(aux1);
		aux1=aux2;
	}	
}

float fifo_time_avg(fifo_node ** back, int current_time){
	fifo_node * aux;
	aux = *back;
	float total = 0;
	item_server * item;
	
	while(aux!=NULL){
		item=(item_server*)aux->item;
		total = total + (current_time-item->time);
		aux=aux->next;
	}	
	
	if(fifo_count == 0){
		return 0;
	}
	
	return  total/fifo_count;
}

