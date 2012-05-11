#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "pool.h"
#include "yasc.h"

pool_node * create_pool(){
	return NULL;
}

void create_pool_node(pool_node ** first, int flag){

	pool_node * new;
	pool_node * aux;
	new = (pool_node *) malloc (sizeof(pool_node));
	new->thread = (pthread_t*) malloc (sizeof(pthread_t));
	pthread_create(new->thread, NULL, yasc, (void*)new);
	
	
	new->time = 0;
	new->next = NULL;
	new->socket = 0;
	new->stack=NULL;
	new->flag = flag;
	pthread_mutex_init(&(new->stackmux),NULL);
	pthread_mutex_lock(&poolmux);
	aux = *first;
	if(aux!= NULL){
		while(aux->next != NULL){ 
			aux=aux->next;
		}
		aux->next=new;
	}else{
		*first = new;
	}
	pthread_mutex_unlock(&poolmux);
	
}

void remove_pool(pool_node ** first){
	pool_node * aux1, *aux2;
	aux1=*first;
	aux2=*first;
	while(aux1!=NULL)
	{
		aux2=aux1->next;
		remove_pool_node(aux1);
		aux1=aux2;
	}

}

void remove_pool_node(pool_node * node){
	FreeStack(node->stack);
	free(node->thread);
	free(node);

}

